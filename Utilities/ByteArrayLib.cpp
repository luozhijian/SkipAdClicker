#include "ByteArrayLib.hpp"

#include <algorithm>

namespace automationtest::utilities {

std::vector<std::byte> ByteArrayLib::CopyToByteArray(const Bitmap& bitmap, int& stride)
{
    stride = bitmap.stride;
    return bitmap.pixels;
}

Bitmap ByteArrayLib::ByteArrayToBitmap(const std::vector<std::byte>& pixel_data, int width, int height, int stride)
{
    const int channels = (width > 0 && stride >= width) ? std::max(1, stride / width) : 1;
    return Bitmap {width, height, stride, pixel_data, channels};
}

void ByteArrayLib::ApplyThresholdOptimized(std::vector<std::byte>& bytes, const std::vector<unsigned char>& desired_colors, int delta)
{
    std::vector<unsigned char> lookup(256, 0);
    for (int color : desired_colors) {
        for (int i = color - delta; i <= color + delta; ++i) {
            if (i >= 0 && i <= 255) {
                lookup[static_cast<std::size_t>(i)] = 255;
            }
        }
    }
    for (auto& value : bytes) {
        value = static_cast<std::byte>(lookup[static_cast<unsigned char>(value)]);
    }
}

std::vector<LineWithDescription> ByteArrayLib::FindHorizontalLines(const std::vector<std::byte>& bytes, int stride, int width, int height, int min_line_length)
{
    std::vector<LineWithDescription> lines;
    for (int y = 0; y < height; ++y) {
        int start_x = -1;
        int length = 0;
        int current = y * stride;
        for (int x = 0; x < width; ++x, ++current) {
            const auto pixel = static_cast<unsigned char>(bytes[current]);
            if (pixel > 0) {
                if (start_x == -1) {
                    start_x = x;
                }
                ++length;
            } else {
                if (length >= min_line_length) {
                    lines.emplace_back(Point {start_x, y}, Point {x - 1, y});
                }
                start_x = -1;
                length = 0;
            }
        }
        if (length >= min_line_length) {
            lines.emplace_back(Point {start_x, y}, Point {width - 1, y});
        }
    }
    return lines;
}

std::vector<LineWithDescription> ByteArrayLib::FindVerticalLines(const std::vector<std::byte>& bytes, int stride, int width, int height, int min_line_length)
{
    std::vector<LineWithDescription> lines;
    for (int x = 0; x < width; ++x) {
        int start_y = -1;
        int length = 0;
        int current = x;
        for (int y = 0; y < height; ++y, current += stride) {
            const auto pixel = static_cast<unsigned char>(bytes[current]);
            if (pixel > 0) {
                if (start_y == -1) {
                    start_y = y;
                }
                ++length;
            } else {
                if (length >= min_line_length) {
                    lines.emplace_back(Point {x, start_y}, Point {x, y - 1});
                }
                start_y = -1;
                length = 0;
            }
        }
        if (length >= min_line_length) {
            lines.emplace_back(Point {x, start_y}, Point {x, height - 1});
        }
    }
    return lines;
}

} // namespace automationtest::utilities
