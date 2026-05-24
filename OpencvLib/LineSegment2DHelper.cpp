#include "LineSegment2DHelper.hpp"

#include <algorithm>

namespace automationtest::opencvlib {

using automationtest::utilities::types::ShortLine;
using automationtest::utilities::types::ShortPoint;

ShortLine LineSegment2DHelper::ToShortPointHorizon(const LineSegment2D& point) noexcept
{
    return ShortLine(
        ShortPoint(std::min(point.p1.x, point.p2.x), std::min(point.p1.y, point.p2.y)),
        ShortPoint(std::max(point.p1.x, point.p2.x), std::max(point.p1.y, point.p2.y)));
}

ShortLine LineSegment2DHelper::ToShortPoint(const LineSegment2D& point) noexcept
{
    return ShortLine(ShortPoint(point.p1.x, point.p1.y), ShortPoint(point.p2.x, point.p2.y));
}

} // namespace automationtest::opencvlib
