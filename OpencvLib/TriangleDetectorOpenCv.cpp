#include "TriangleDetectorOpenCv.hpp"

#include "TriangleDetector.hpp"

namespace automationtest::opencvlib {

TriangleDetectorOpenCv::TriangleDetectorOpenCv(const automationtest::utilities::Bitmap& bitmap)
    : bitmap_(bitmap)
{
}

std::vector<automationtest::utilities::TriangleWithDescription> TriangleDetectorOpenCv::FindTriangles(const automationtest::utilities::settings::SettingTriangleDetection& triangle_setting) const
{
    return TriangleDetector(bitmap_).FindTriangles(triangle_setting);
}

} // namespace automationtest::opencvlib
