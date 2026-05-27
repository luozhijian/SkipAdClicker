#include "ShortLine.hpp"

#include <algorithm>

namespace automationtest::utilities::types {

ShortLine::ShortLine() noexcept
    : long_value(0)
{
}

ShortLine::ShortLine(ShortPoint start, ShortPoint end) noexcept
    : long_value(0)
{
    p1 = start.packed_value;
    p2 = end.packed_value;
}

ShortPoint ShortLine::Point1() const noexcept
{
    return ShortPoint(x1, y1);
}

ShortPoint ShortLine::Point2() const noexcept
{
    return ShortPoint(x2, y2);
}

std::tuple<std::int16_t, std::int16_t, std::int16_t, std::int16_t> ShortLine::ToShortXY() const noexcept
{
    return {x1, y1, x2, y2};
}

std::pair<ShortPoint, ShortPoint> ShortLine::ToShortPoints() const noexcept
{
    return {Point1(), Point2()};
}

std::int16_t ShortLine::MinX() const noexcept
{
    return std::min(x1, x2);
}

std::int16_t ShortLine::MaxX() const noexcept
{
    return std::max(x1, x2);
}

std::int16_t ShortLine::MinY() const noexcept
{
    return std::min(y1, y2);
}

std::int16_t ShortLine::MaxY() const noexcept
{
    return std::max(y1, y2);
}

} // namespace automationtest::utilities::types
