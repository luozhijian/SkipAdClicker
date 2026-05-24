#ifndef AUTOMATIOTEST_OPENCVLIB_RECTANGLEDETECTOR_HPP
#define AUTOMATIOTEST_OPENCVLIB_RECTANGLEDETECTOR_HPP

#include "../Utilities/CommonTypes.hpp"
#include "../Utilities/LineWithDescription.hpp"

#include <map>
#include <vector>

namespace automationtest::opencvlib {

class RectangleDetector {
public:
    std::vector<automationtest::utilities::Rectangle> ExtractRectanglesFromLines(const std::vector<automationtest::utilities::LineWithDescription>& lines, double delta_x = 2.0, double delta_y = 2.0);
};

} // namespace automationtest::opencvlib

#endif // AUTOMATIOTEST_OPENCVLIB_RECTANGLEDETECTOR_HPP
