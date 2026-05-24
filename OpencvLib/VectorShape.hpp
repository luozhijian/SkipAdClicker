#ifndef AUTOMATIOTEST_OPENCVLIB_VECTORSHAPE_HPP
#define AUTOMATIOTEST_OPENCVLIB_VECTORSHAPE_HPP

#include "../Utilities/CommonTypes.hpp"
#include "../Utilities/LineWithDescription.hpp"

#include <vector>

namespace automationtest::opencvlib {

class VectorShape {
public:
    static automationtest::utilities::Rectangle IsHorizonVerticalRectangle(const std::vector<automationtest::utilities::Point>& points);
    static std::vector<automationtest::utilities::LineWithDescription> GroupVectorIntoLine(const std::vector<automationtest::utilities::Point>& points, int max_allowance);
};

} // namespace automationtest::opencvlib

#endif // AUTOMATIOTEST_OPENCVLIB_VECTORSHAPE_HPP
