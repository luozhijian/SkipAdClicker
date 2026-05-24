#ifndef AUTOMATIOTEST_OPENCVLIB_LINESEGMENT2D_HPP
#define AUTOMATIOTEST_OPENCVLIB_LINESEGMENT2D_HPP

#include "../Utilities/CommonTypes.hpp"

#include <cmath>

namespace automationtest::opencvlib {

struct LineSegment2D {
    automationtest::utilities::Point p1 {};
    automationtest::utilities::Point p2 {};

    [[nodiscard]] double Length() const noexcept
    {
        const auto dx = static_cast<double>(p2.x - p1.x);
        const auto dy = static_cast<double>(p2.y - p1.y);
        return std::sqrt(dx * dx + dy * dy);
    }
};

} // namespace automationtest::opencvlib

#endif // AUTOMATIOTEST_OPENCVLIB_LINESEGMENT2D_HPP
