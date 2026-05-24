#include "CommonTypes.hpp"

#include <sstream>

namespace automationtest::utilities {

std::string Point::ToString() const
{
    std::ostringstream builder;
    builder << "{X=" << x << ",Y=" << y << "}";
    return builder.str();
}

bool Size::IsEmpty() const noexcept
{
    return width <= 0 || height <= 0;
}

int Rectangle::Left() const noexcept
{
    return x;
}

int Rectangle::Top() const noexcept
{
    return y;
}

int Rectangle::Right() const noexcept
{
    return x + width;
}

int Rectangle::Bottom() const noexcept
{
    return y + height;
}

bool Rectangle::IsEmpty() const noexcept
{
    return width <= 0 || height <= 0;
}

std::ostream& operator<<(std::ostream& stream, const Point& point)
{
    return stream << point.ToString();
}

std::ostream& operator<<(std::ostream& stream, const Rectangle& rectangle)
{
    return stream << "{X=" << rectangle.x
                  << ",Y=" << rectangle.y
                  << ",Width=" << rectangle.width
                  << ",Height=" << rectangle.height
                  << "}";
}

std::ostream& operator<<(std::ostream& stream, const Color& color)
{
    return stream << "RGB(" << static_cast<int>(color.r)
                  << "," << static_cast<int>(color.g)
                  << "," << static_cast<int>(color.b)
                  << ")";
}

} // namespace automationtest::utilities
