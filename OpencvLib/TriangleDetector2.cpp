#include "TriangleDetector2.hpp"

#include "TriangleDetector.hpp"

namespace automationtest::opencvlib {

TriangleDetector2::TriangleDetector2(const std::vector<std::byte>& gray_values, int stride, int height, int width)
    : values_(gray_values),
      stride_(stride),
      height_(height),
      width_(width)
{
}

std::vector<automationtest::utilities::TriangleWithDescription> TriangleDetector2::FindTriangles() const
{
    automationtest::utilities::Bitmap bitmap {};
    bitmap.width = width_;
    bitmap.height = height_;
    bitmap.stride = stride_;
    bitmap.channels = 1;
    bitmap.pixels = values_;
    return TriangleDetector(bitmap).FindTriangles({});
}

} // namespace automationtest::opencvlib
