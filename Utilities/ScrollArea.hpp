#ifndef AUTOMATIOTEST_SCROLLAREA_HPP
#define AUTOMATIOTEST_SCROLLAREA_HPP

#include "CommonTypes.hpp"
#include "ScrollbarWithDescription.hpp"

#include <functional>
#include <optional>

namespace automationtest::utilities {

class ScrollArea {
public:
    Point location {};
    Rectangle rectangle {};
    std::optional<ScrollbarWithDescription> vertical_scrollbar {};
    std::optional<ScrollbarWithDescription> horizontal_scrollbar {};
    Rectangle content_area {};

    ScrollArea& CalculateContentArea() noexcept;
    Bitmap ClickScrollDown(const std::function<Bitmap(Point, int)>& mouse_click, int times_to_click) const;
    ScrollArea& AdjustScrollArea(const Rectangle& large, const Rectangle& small) noexcept;
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_SCROLLAREA_HPP
