#include "ShortPoint.hpp"

namespace automationtest::utilities::types {

ShortPoint::ShortPoint() noexcept
    : packed_value(0)
{
}

ShortPoint::ShortPoint(int x_value, int y_value) noexcept
    : x(static_cast<std::int16_t>(x_value))
    , y(static_cast<std::int16_t>(y_value))
{
}

automationtest::utilities::Point ShortPoint::ToPoint() const noexcept
{
    return automationtest::utilities::Point {x, y};
}

} // namespace automationtest::utilities::types
