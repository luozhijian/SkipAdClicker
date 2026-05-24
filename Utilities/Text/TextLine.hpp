#ifndef AUTOMATIOTEST_TEXT_TEXTLINE_HPP
#define AUTOMATIOTEST_TEXT_TEXTLINE_HPP

#include "../CommonTypes.hpp"

#include <vector>

namespace automationtest::utilities::text {

struct TextLine {
    std::vector<automationtest::utilities::Rectangle> text_blocks {};
    int y {0};
    int height {0};
};

} // namespace automationtest::utilities::text

#endif // AUTOMATIOTEST_TEXT_TEXTLINE_HPP
