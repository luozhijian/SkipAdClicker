#ifndef AUTOMATIOTEST_SCROLLBARWITHDESCRIPTION_HPP
#define AUTOMATIOTEST_SCROLLBARWITHDESCRIPTION_HPP

#include "CommonTypes.hpp"
#include "PloygonWithDescription.hpp"
#include "TriangleWithDescription.hpp"
#include "eOrientation.hpp"

#include <functional>

namespace automationtest::utilities {

class ScrollbarWithDescription : public PloygonWithDescription {
public:
    eOrientation orientation {eOrientation::Horizontal};
    TriangleWithDescription triangle1 {};
    TriangleWithDescription triangle2 {};
    Rectangle scroll_bar_rectangle {};

    Bitmap Click(Point location, const std::function<Bitmap(Point, int)>& mouse_click, int times_to_click, bool is_first, int wait_milliseconds = 200) const;
    void Shift(const Size& size) noexcept;
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_SCROLLBARWITHDESCRIPTION_HPP
