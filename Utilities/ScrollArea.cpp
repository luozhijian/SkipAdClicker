#include "ScrollArea.hpp"

namespace automationtest::utilities {

ScrollArea& ScrollArea::CalculateContentArea() noexcept
{
    const int top = rectangle.Top() + 1;
    const int left = rectangle.Left() + 1;
    const int right = vertical_scrollbar.has_value() ? vertical_scrollbar->scroll_bar_rectangle.Left() - 1 : rectangle.Right() - 1;
    const int bottom = horizontal_scrollbar.has_value() ? horizontal_scrollbar->scroll_bar_rectangle.Bottom() - 1 : rectangle.Bottom() - 1;
    content_area = Rectangle {left, top, right - left, bottom - top};
    return *this;
}

Bitmap ScrollArea::ClickScrollDown(const std::function<Bitmap(Point, int)>& mouse_click, int times_to_click) const
{
    if (vertical_scrollbar.has_value()) {
        return vertical_scrollbar->Click(location, mouse_click, times_to_click, false);
    }
    return Bitmap {};
}

ScrollArea& ScrollArea::AdjustScrollArea(const Rectangle& large, const Rectangle& small) noexcept
{
    const Size size {small.Left() - large.Left(), small.Top() - large.Top()};
    rectangle = large;
    if (vertical_scrollbar.has_value()) {
        vertical_scrollbar->Shift(size);
    }
    if (horizontal_scrollbar.has_value()) {
        horizontal_scrollbar->Shift(size);
    }
    return CalculateContentArea();
}

} // namespace automationtest::utilities
