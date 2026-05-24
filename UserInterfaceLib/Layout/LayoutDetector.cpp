#include "LayoutDetector.hpp"

#include "../TextBlockDetector.hpp"
#include "../../Utilities/BitmapHelper.hpp"
#include "../../Utilities/ByteArrayLib.hpp"
#include "../../Utilities/RectangleHelper.hpp"

#include <algorithm>
#include <numeric>
#include <unordered_map>

namespace automationtest::userinterfacelib {

using automationtest::utilities::BitmapHelper;
using automationtest::utilities::ByteArrayLib;
using automationtest::utilities::RectangleHelper;

LayoutDetector::LayoutDetector(const Bitmap& bitmap)
    : bitmap_(BitmapHelper::ConvertToGrayscale(bitmap))
{
    gray_values_ = ByteArrayLib::CopyToByteArray(bitmap_, stride_);
    width_ = bitmap_.width;
    height_ = bitmap_.height;
}

LayoutDetector::LayoutDetector(const std::vector<std::byte>& gray_values, int stride, int width, int height)
    : gray_values_(gray_values),
      stride_(stride),
      width_(width),
      height_(height)
{
    bitmap_.width = width_;
    bitmap_.height = height_;
    bitmap_.stride = stride_;
    bitmap_.channels = 1;
    bitmap_.pixels = gray_values_;
}

Rectangle LayoutDetector::GetContentRectangle(int min_length) const
{
    if (gray_values_.empty()) {
        return {};
    }

    std::unordered_map<std::uint8_t, int> histogram {};
    for (const auto& value : gray_values_) {
        ++histogram[static_cast<std::uint8_t>(value)];
    }

    const auto background = std::max_element(histogram.begin(), histogram.end(), [](const auto& left, const auto& right) {
        return left.second < right.second;
    })->first;

    std::unordered_map<int, int> runs {};
    for (int row = 0; row < height_; ++row) {
        int count = 0;
        int first_x = 0;
        for (int col = 0; col < width_; ++col) {
            const auto index = static_cast<std::size_t>(row) * static_cast<std::size_t>(stride_) + static_cast<std::size_t>(col);
            const auto is_same = static_cast<std::uint8_t>(gray_values_[index]) == background;
            if (is_same) {
                if (count == 0) {
                    first_x = col;
                }
                ++count;
            }
            if (!is_same || col == width_ - 1) {
                if (count >= min_length) {
                    const auto end_col = col - (is_same ? 0 : 1);
                    runs[first_x * width_ + end_col] += 1;
                }
                count = 0;
            }
        }
    }

    if (runs.empty()) {
        return Rectangle {0, 0, width_, height_};
    }

    const auto best = std::max_element(runs.begin(), runs.end(), [](const auto& left, const auto& right) {
        return left.second < right.second;
    })->first;
    const auto start_x = best / width_;
    const auto end_x = best % width_;
    return Rectangle {start_x, 0, std::max(0, end_x - start_x), height_};
}

Rectangle LayoutDetector::FindTopPart(const Bitmap& bitmap)
{
    return RectangleHelper::FromTwoPoints(Point {0, 0}, Point {bitmap.width, std::min(bitmap.height, 400)});
}

std::vector<Rectangle> LayoutDetector::AdjustLayoutAs(const Rectangle& popup, const std::vector<Rectangle>& rectangles, int items_per_row)
{
    std::vector<Rectangle> result {};
    if (rectangles.empty() || items_per_row <= 0) {
        return result;
    }

    auto sorted = rectangles;
    std::sort(sorted.begin(), sorted.end(), [](const auto& left, const auto& right) {
        if (left.y != right.y) {
            return left.y < right.y;
        }
        return left.x < right.x;
    });

    for (std::size_t index = 0; index < sorted.size(); index += static_cast<std::size_t>(items_per_row)) {
        const auto end = std::min(sorted.size(), index + static_cast<std::size_t>(items_per_row));
        const std::vector<Rectangle> line(sorted.begin() + static_cast<std::ptrdiff_t>(index), sorted.begin() + static_cast<std::ptrdiff_t>(end));
        const auto left = std::min_element(line.begin(), line.end(), [](const auto& a, const auto& b) { return a.x < b.x; })->x;
        const auto top = std::min_element(line.begin(), line.end(), [](const auto& a, const auto& b) { return a.y < b.y; })->y;
        const auto right = std::max_element(line.begin(), line.end(), [](const auto& a, const auto& b) { return a.x + a.width < b.x + b.width; })->x
            + std::max_element(line.begin(), line.end(), [](const auto& a, const auto& b) { return a.x + a.width < b.x + b.width; })->width;
        const auto bottom = std::max_element(line.begin(), line.end(), [](const auto& a, const auto& b) { return a.y + a.height < b.y + b.height; })->y
            + std::max_element(line.begin(), line.end(), [](const auto& a, const auto& b) { return a.y + a.height < b.y + b.height; })->height;
        const auto rectangle = RectangleHelper::FromXYXY(left, top, right, bottom);
        result.push_back(RectangleHelper::Shift(rectangle, Point {popup.x, popup.y}));
    }
    return result;
}

std::vector<TextMatch> LayoutDetector::FindTextWithSpecificLayout(const Bitmap& bitmap, int items_per_row, int min_size, const TextProvider& provider)
{
    std::vector<TextMatch> result {};
    const auto text_blocks = TextBlockDetector::FindTextBlocks(bitmap, min_size);
    for (const auto& rectangle : AdjustLayoutAs(Rectangle {0, 0, bitmap.width, bitmap.height}, text_blocks, items_per_row)) {
        auto text = provider ? provider(BitmapHelper::GetBitmap(bitmap, rectangle)) : std::string {};
        result.push_back(TextMatch {rectangle, std::move(text)});
    }
    return result;
}

Rectangle LayoutDetector::FigureSizeFromLocation(const std::vector<std::string>& locations, const Size& container_size) const
{
    if (locations.size() == 1 && locations.front() == "TopLeft") {
        return Rectangle {0, 0, std::max(container_size.width / 5, 300), std::max(container_size.height / 8, 200)};
    }
    return Rectangle {0, 0, container_size.width, container_size.height};
}

} // namespace automationtest::userinterfacelib
