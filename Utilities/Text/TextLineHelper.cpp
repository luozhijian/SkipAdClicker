#include "TextLineHelper.hpp"

#include <algorithm>

namespace automationtest::utilities::text {

std::optional<TextLine> TextLineHelper::FindRelatedTextLine(const std::vector<TextLine>& text_lines, const automationtest::utilities::Rectangle& rect)
{
    for (const auto& line : text_lines) {
        const auto it = std::find(line.text_blocks.begin(), line.text_blocks.end(), rect);
        if (it != line.text_blocks.end()) {
            return line;
        }
    }
    return std::nullopt;
}

} // namespace automationtest::utilities::text
