#ifndef AUTOMATIOTEST_TEXT_TEXTLINEHELPER_HPP
#define AUTOMATIOTEST_TEXT_TEXTLINEHELPER_HPP

#include "TextLine.hpp"

#include <optional>
#include <vector>

namespace automationtest::utilities::text {

class TextLineHelper {
public:
    static std::optional<TextLine> FindRelatedTextLine(const std::vector<TextLine>& text_lines, const automationtest::utilities::Rectangle& rect);
};

} // namespace automationtest::utilities::text

#endif // AUTOMATIOTEST_TEXT_TEXTLINEHELPER_HPP
