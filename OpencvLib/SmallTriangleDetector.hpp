#ifndef AUTOMATIOTEST_OPENCVLIB_SMALLTRIANGLEDETECTOR_HPP
#define AUTOMATIOTEST_OPENCVLIB_SMALLTRIANGLEDETECTOR_HPP

#include "../Utilities/CommonTypes.hpp"

#include <vector>

namespace automationtest::opencvlib {

class SmallTriangleDetector {
public:
    explicit SmallTriangleDetector(const automationtest::utilities::Bitmap& bitmap);

    [[nodiscard]] std::vector<automationtest::utilities::Rectangle> FindSmallTriangles() const;

private:
    automationtest::utilities::Bitmap bitmap_ {};
};

} // namespace automationtest::opencvlib

#endif // AUTOMATIOTEST_OPENCVLIB_SMALLTRIANGLEDETECTOR_HPP
