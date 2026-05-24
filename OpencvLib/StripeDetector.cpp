#include "StripeDetector.hpp"

#include "MatExtension.hpp"
#include "OpenCvLib.hpp"

#include <algorithm>

namespace automationtest::opencvlib {

using automationtest::utilities::Rectangle;

cv::Mat StripeDetector::ErodeInverted(const cv::Mat& cannied, int kernel_w, int kernel_h)
{
    if (cannied.empty() || kernel_w <= 0 || kernel_h <= 0 || kernel_w > cannied.cols || kernel_h > cannied.rows) {
        return {};
    }

    cv::Mat output(cannied.rows, cannied.cols, CV_8UC1);
    const int half_w = kernel_w / 2;
    const int half_h = kernel_h / 2;
    for (int row = 0; row < cannied.rows; ++row) {
        for (int col = 0; col < cannied.cols; ++col) {
            bool all_free = true;
            for (int y = row - half_h; y < row - half_h + kernel_h && all_free; ++y) {
                if (y < 0 || y >= cannied.rows) {
                    all_free = false;
                    break;
                }
                for (int x = col - half_w; x < col - half_w + kernel_w; ++x) {
                    if (x < 0 || x >= cannied.cols || MatExtension::GetByteValue(cannied, y, x) != 0) {
                        all_free = false;
                        break;
                    }
                }
            }
            MatExtension::SetByteValue(output, row, col, all_free ? 255 : 0);
        }
    }

	OpenCvLib::DebugSaveMatAsBitmapFile(output);
    return output;
}

bool StripeDetector::DetectIfStripExists(const cv::Mat& cannied, int kernel_w, int kernel_h)
{
    return FindAnyStripArea(ErodeInverted(cannied, kernel_w, kernel_h));
}

std::vector<Rectangle> StripeDetector::Detect(const cv::Mat& cannied, int kernel_w, int kernel_h, int min_area)
{
    return FindMaxRectanglesOfWhiteArea(ErodeInverted(cannied, kernel_w, kernel_h), min_area);
}

bool StripeDetector::FindAnyStripArea(const cv::Mat& binary)
{
    if (binary.empty()) {
        return false;
    }

    for (int row = 0; row < binary.rows; ++row) {
        for (int col = 0; col < binary.cols; ++col) {
            if (MatExtension::GetByteValue(binary, row, col) > 0) {
                return true;
            }
        }
    }
    return false;
}

std::vector<Rectangle> StripeDetector::FindMaxRectanglesOfWhiteArea(const cv::Mat& binary, int min_area)
{
    std::vector<Rectangle> candidates;
    if (binary.empty()) {
        return candidates;
    }

    std::vector<int> heights(static_cast<std::size_t>(binary.cols), 0);
    for (int row = 0; row < binary.rows; ++row) {
        for (int col = 0; col < binary.cols; ++col) {
            heights[static_cast<std::size_t>(col)] = MatExtension::GetByteValue(binary, row, col) > 0 ? heights[static_cast<std::size_t>(col)] + 1 : 0;
        }

        std::vector<std::pair<int, int>> stack;
        for (int col = 0; col <= binary.cols; ++col) {
            const int current_height = col < binary.cols ? heights[static_cast<std::size_t>(col)] : 0;
            int start = col;
            while (!stack.empty() && stack.back().second > current_height) {
                const auto [start_x, height] = stack.back();
                stack.pop_back();
                const int width = col - start_x;
                if (width * height >= min_area) {
                    candidates.push_back(Rectangle {start_x, row - height + 1, width, height});
                }
                start = start_x;
            }
            stack.emplace_back(start, current_height);
        }
    }

    return RemoveContainedRectangles(std::move(candidates));
}

std::vector<Rectangle> StripeDetector::RemoveContainedRectangles(std::vector<Rectangle> candidates)
{
    std::ranges::sort(candidates, [](const Rectangle& left, const Rectangle& right) {
        return left.width * left.height > right.width * right.height;
    });

    std::vector<Rectangle> result;
    for (const auto& candidate : candidates) {
        const bool inside_larger = std::any_of(result.begin(), result.end(), [&](const Rectangle& larger) {
            return candidate.Left() >= larger.Left() && candidate.Right() <= larger.Right()
                && candidate.Top() >= larger.Top() && candidate.Bottom() <= larger.Bottom();
        });
        if (!inside_larger) {
            result.push_back(candidate);
        }
    }

    return result;
}

} // namespace automationtest::opencvlib
