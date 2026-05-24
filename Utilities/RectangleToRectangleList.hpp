#ifndef AUTOMATIOTEST_RECTANGLETORECTANGLELIST_HPP
#define AUTOMATIOTEST_RECTANGLETORECTANGLELIST_HPP

#include "CommonTypes.hpp"

#include <string>
#include <vector>

namespace automationtest::utilities {

struct RectangleToRectangleList {
    std::string text {};
    Rectangle rect {};
    std::vector<Rectangle> list {};

    void AddRectangle(Rectangle rectangle);
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_RECTANGLETORECTANGLELIST_HPP
