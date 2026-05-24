#ifndef AUTOMATIOTEST_USERINTERFACELIB_LAYOUT_SCREENLAYOUT_HPP
#define AUTOMATIOTEST_USERINTERFACELIB_LAYOUT_SCREENLAYOUT_HPP

#include "../../Utilities/Common.hpp"

#include <string>
#include <unordered_map>

namespace automationtest::userinterfacelib {

class ScreenLayout {
public:
    ScreenLayout() = default;
    explicit ScreenLayout(std::string layout_name);

    std::string name {};
    std::unordered_map<std::string, Rectangle> layouts {};
    Rectangle rectangle {};
    Point top_left {};
};

} // namespace automationtest::userinterfacelib

#endif // AUTOMATIOTEST_USERINTERFACELIB_LAYOUT_SCREENLAYOUT_HPP
