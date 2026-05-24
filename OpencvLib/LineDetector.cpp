#include "LineDetector.hpp"

#include "../Utilities/BitmapHelper.hpp"
#include "../Utilities/ByteArrayLib.hpp"

#include <algorithm>
#include <set>

namespace automationtest::opencvlib {

using automationtest::utilities::Bitmap;
using automationtest::utilities::BitmapHelper;
using automationtest::utilities::ByteArrayLib;
using automationtest::utilities::LineWithDescription;
using automationtest::utilities::Point;

LineDetector::LineDetector(const Bitmap& bitmap)
{
    const auto gray = BitmapHelper::ConvertToGrayscale(bitmap);
    values_ = ByteArrayLib::CopyToByteArray(gray, stride_);
    width_ = gray.width;
    height_ = gray.height;
    background_color_ = 0;
}

LineDetector::LineDetector(const std::vector<std::byte>& gray_values, int stride, int width, int height, std::uint8_t background_color)
    : values_(gray_values),
      stride_(stride),
      width_(width),
      height_(height),
      background_color_(background_color)
{
}

std::vector<LineWithDescription> LineDetector::FindLines(int threshold) const
{
    std::vector<LineWithDescription> lines = FindAllHorizonLines(threshold, true);
    auto verticals = FindAllVerticalLines(threshold, true);
    lines.insert(lines.end(), verticals.begin(), verticals.end());
    std::sort(lines.begin(), lines.end(), [](const auto& left, const auto& right) {
        if (left.Point1().y != right.Point1().y) {
            return left.Point1().y < right.Point1().y;
        }
        return left.Point1().x < right.Point1().x;
    });
    return lines;
}

std::vector<LineWithDescription> LineDetector::FindAllHorizonLines(int threshold, bool explicit_line) const
{
    std::vector<LineWithDescription> result {};
    for (int row = 0; row < height_; ++row) {
        int column = 0;
        while (column < width_ - threshold) {
            const auto color = PixelAt(row, column);
            if (explicit_line && color == background_color_) {
                ++column;
                continue;
            }

            int end = column + 1;
            while (end < width_ && PixelAt(row, end) == color) {
                ++end;
            }

            if (end - column >= threshold) {
                result.emplace_back(Point {column, row}, Point {end - 1, row}, color);
            }
            column = std::max(end, column + 1);
        }
    }
    return result;
}

std::vector<LineWithDescription> LineDetector::FindAllVerticalLines(int threshold, bool explicit_line) const
{
    std::vector<LineWithDescription> result {};
    for (int column = 0; column < width_; ++column) {
        int row = 0;
        while (row < height_ - threshold) {
            const auto color = PixelAt(row, column);
            if (explicit_line && color == background_color_) {
                ++row;
                continue;
            }

            int end = row + 1;
            while (end < height_ && PixelAt(end, column) == color) {
                ++end;
            }

            if (end - row >= threshold) {
                result.emplace_back(Point {column, row}, Point {column, end - 1}, color);
            }
            row = std::max(end, row + 1);
        }
    }
    return result;
}

std::uint8_t LineDetector::PixelAt(int row, int column) const
{
    return static_cast<std::uint8_t>(values_[static_cast<std::size_t>(row) * static_cast<std::size_t>(stride_) + static_cast<std::size_t>(column)]);
}

bool LineDetector::InBounds(int row, int column) const noexcept
{
    return row >= 0 && row < height_ && column >= 0 && column < width_;
}

} // namespace automationtest::opencvlib
