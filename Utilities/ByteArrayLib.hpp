#ifndef AUTOMATIOTEST_BYTEARRAYLIB_HPP
#define AUTOMATIOTEST_BYTEARRAYLIB_HPP

#include "BitmapHelper.hpp"
#include "LineWithDescription.hpp"

#include <cstddef>
#include <vector>

namespace automationtest::utilities {

class ByteArrayLib {
public:
    static std::vector<std::byte> CopyToByteArray(const Bitmap& bitmap, int& stride);
    static Bitmap ByteArrayToBitmap(const std::vector<std::byte>& pixel_data, int width, int height, int stride);
    static void ApplyThresholdOptimized(std::vector<std::byte>& bytes, const std::vector<unsigned char>& desired_colors, int delta);
    static std::vector<LineWithDescription> FindHorizontalLines(const std::vector<std::byte>& bytes, int stride, int width, int height, int min_line_length);
    static std::vector<LineWithDescription> FindVerticalLines(const std::vector<std::byte>& bytes, int stride, int width, int height, int min_line_length);
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_BYTEARRAYLIB_HPP
