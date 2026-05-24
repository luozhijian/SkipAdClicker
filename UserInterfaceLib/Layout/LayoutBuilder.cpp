#include "LayoutBuilder.hpp"

#include "../TextBlockDetector.hpp"

namespace automationtest::userinterfacelib {

ScreenLayout LayoutBuilder::BuildByTextBlock(const Bitmap& bitmap, const std::string& layout_name, const std::vector<LayoutPartRule>& rules, const std::vector<TextMatch>& detected_text)
{
    ScreenLayout result(layout_name);
    result.rectangle = Rectangle {0, 0, bitmap.width, bitmap.height};

    auto candidates = detected_text;
    if (candidates.empty()) {
        for (const auto& rectangle : TextBlockDetector::FindTextBlocks(bitmap)) {
            candidates.push_back(TextMatch {rectangle, {}});
        }
    }

    for (const auto& rule : rules) {
        for (const auto& candidate : candidates) {
            if (!candidate.text.empty() && std::regex_search(candidate.text, rule.pattern)) {
                result.layouts[rule.name] = candidate.rectangle;
                break;
            }
        }
    }

    return result;
}

std::vector<TextMatch> LayoutBuilder::MatchTextBlocksWithRegex(const std::vector<TextMatch>& text_blocks, const std::vector<std::regex>& regexes) const
{
    std::vector<TextMatch> matches {};
    for (const auto& block : text_blocks) {
        for (const auto& regex : regexes) {
            if (std::regex_search(block.text, regex)) {
                matches.push_back(block);
                break;
            }
        }
    }
    return matches;
}

} // namespace automationtest::userinterfacelib
