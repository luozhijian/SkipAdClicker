#include "ScrollbarWithDescription.hpp"

namespace automationtest::utilities {

Bitmap ScrollbarWithDescription::Click(Point location, const std::function<Bitmap(Point, int)>& mouse_click, int times_to_click, bool is_first, int wait_milliseconds) const
{
    Point location1 = location;
    const auto center = is_first ? triangle1.Center() : triangle2.Center();
    location1.x += center.x;
    location1.y += center.y;
    Bitmap result {};
    if (times_to_click <= 1) {
        times_to_click = 2;
    }
    while (times_to_click-- > 0) {
        result = mouse_click(location1, wait_milliseconds);
    }
    return result;
}

void ScrollbarWithDescription::Shift(const Size& size) noexcept
{
    triangle1.Shift(size);
    triangle2.Shift(size);
}

} // namespace automationtest::utilities
