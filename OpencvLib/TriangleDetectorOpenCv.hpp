#ifndef AUTOMATIOTEST_OPENCVLIB_TRIANGLEDETECTOROPENCV_HPP
#define AUTOMATIOTEST_OPENCVLIB_TRIANGLEDETECTOROPENCV_HPP

#include "../Utilities/Settings/SettingTriangleDetection.hpp"

#include "../Utilities/CommonTypes.hpp"
#include "../Utilities/TriangleWithDescription.hpp"

#include <vector>

namespace automationtest::opencvlib {

class TriangleDetectorOpenCv {
public:
    explicit TriangleDetectorOpenCv(const automationtest::utilities::Bitmap& bitmap);

    [[nodiscard]] std::vector<automationtest::utilities::TriangleWithDescription> FindTriangles(const automationtest::utilities::settings::SettingTriangleDetection& triangle_setting) const;

private:
    automationtest::utilities::Bitmap bitmap_ {};
};

} // namespace automationtest::opencvlib

#endif // AUTOMATIOTEST_OPENCVLIB_TRIANGLEDETECTOROPENCV_HPP
