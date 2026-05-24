#ifndef AUTOMATIOTEST_OPENCVLIB_LINESEGMENT2DHELPER_HPP
#define AUTOMATIOTEST_OPENCVLIB_LINESEGMENT2DHELPER_HPP

#include "LineSegment2D.hpp"
#include "../Utilities/Types/ShortLine.hpp"

namespace automationtest::opencvlib {

class LineSegment2DHelper {
public:
    static automationtest::utilities::types::ShortLine ToShortPointHorizon(const LineSegment2D& point) noexcept;
    static automationtest::utilities::types::ShortLine ToShortPoint(const LineSegment2D& point) noexcept;
};

} // namespace automationtest::opencvlib

#endif // AUTOMATIOTEST_OPENCVLIB_LINESEGMENT2DHELPER_HPP
