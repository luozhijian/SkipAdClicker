#ifndef AUTOMATIOTEST_USERINTERFACELIB_LAYOUT_SCREENLAYOUTHELPER_HPP
#define AUTOMATIOTEST_USERINTERFACELIB_LAYOUT_SCREENLAYOUTHELPER_HPP

#include "ScreenLayout.hpp"

#include <stdexcept>
#include <string>

namespace automationtest::userinterfacelib {

class ScreenLayoutHelper {
public:
    static Rectangle LayoutGetPart(const ScreenLayout& layout, const std::string& part_name);
};

} // namespace automationtest::userinterfacelib

#endif // AUTOMATIOTEST_USERINTERFACELIB_LAYOUT_SCREENLAYOUTHELPER_HPP
