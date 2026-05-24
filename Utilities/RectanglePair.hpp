#ifndef AUTOMATIOTEST_RECTANGLEPAIR_HPP
#define AUTOMATIOTEST_RECTANGLEPAIR_HPP

#include "CommonTypes.hpp"

#include <string>

namespace automationtest::utilities {

struct RectanglePair {
    std::string text {};
    Rectangle rect1 {};
    Rectangle rect2 {};

    RectanglePair() = default;
    RectanglePair(Rectangle first, Rectangle second)
        : rect1(first)
        , rect2(second)
    {
    }
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_RECTANGLEPAIR_HPP
