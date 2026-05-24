#include "PointWithAngleAndDistance.hpp"

#include <limits>

namespace automationtest::utilities {

PointWithAngleAndDistance::PointWithAngleAndDistance(Point point_value)
    : PointWithAngleAndDistance(point_value.x, point_value.y)
{
}

PointWithAngleAndDistance::PointWithAngleAndDistance(int x, int y)
    : point_({x, y})
    , radius_squared_(static_cast<double>(x * x + y * y))
    , theta_(x == 0 ? std::numeric_limits<double>::max() : static_cast<double>(y) / static_cast<double>(x))
{
}

const Point& PointWithAngleAndDistance::Value() const noexcept
{
    return point_;
}

double PointWithAngleAndDistance::RadiusSquared() const noexcept
{
    return radius_squared_;
}

double PointWithAngleAndDistance::Theta() const noexcept
{
    return theta_;
}

} // namespace automationtest::utilities
