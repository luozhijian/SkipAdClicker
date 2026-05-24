#ifndef AUTOMATIOTEST_USERINTERFACELIB_LAYOUT_LAYOUTBUILDER_HPP
#define AUTOMATIOTEST_USERINTERFACELIB_LAYOUT_LAYOUTBUILDER_HPP

#include "ScreenLayout.hpp"
#include "../../Utilities/Common.hpp"

#include <regex>
#include <string>
#include <vector>

namespace automationtest::userinterfacelib {

struct LayoutPartRule {
    std::string name {};
    std::regex pattern {};
};

class LayoutBuilder {
public:
    static ScreenLayout BuildByTextBlock(const Bitmap& bitmap, const std::string& layout_name, const std::vector<LayoutPartRule>& rules, const std::vector<TextMatch>& detected_text = {});
    std::vector<TextMatch> MatchTextBlocksWithRegex(const std::vector<TextMatch>& text_blocks, const std::vector<std::regex>& regexes) const;
};

} // namespace automationtest::userinterfacelib

#endif // AUTOMATIOTEST_USERINTERFACELIB_LAYOUT_LAYOUTBUILDER_HPP
