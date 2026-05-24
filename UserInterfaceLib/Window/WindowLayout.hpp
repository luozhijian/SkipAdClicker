#ifndef AUTOMATIOTEST_USERINTERFACELIB_WINDOW_WINDOWLAYOUT_HPP
#define AUTOMATIOTEST_USERINTERFACELIB_WINDOW_WINDOWLAYOUT_HPP

#include "../../Utilities/Common.hpp"

#include <vector>

namespace automationtest::userinterfacelib {

class WindowLayout {
public:
    Size size {};
    std::vector<WindowLayout> layouts {};
    Point relative_location_in_parent_control {};
};

} // namespace automationtest::userinterfacelib

#endif // AUTOMATIOTEST_USERINTERFACELIB_WINDOW_WINDOWLAYOUT_HPP
