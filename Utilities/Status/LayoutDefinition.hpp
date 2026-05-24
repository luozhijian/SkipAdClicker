#ifndef AUTOMATIOTEST_STATUS_LAYOUTDEFINITION_HPP
#define AUTOMATIOTEST_STATUS_LAYOUTDEFINITION_HPP

#include "../CommonTypes.hpp"
#include "../Types/EnumSizeDescription.hpp"

#include <functional>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace automationtest::utilities::status {

class LayoutDefinition {
public:
    std::string name {};
    std::string regex {};
    std::string relative_to {};
    std::string relative_location {};
    std::string location {};
    std::string setting_text_block_setting {};
    std::string setting_line_detection {};
    std::string setting_rectangle_detection {};
    types::EnumSizeDescription size_description {types::EnumSizeDescription::Full};
    int layout_solve_sequence {0};
    bool is_separate_line {false};
    bool is_rectangle {false};
    std::vector<LayoutDefinition> layouts {};
    std::vector<automationtest::utilities::Rectangle> possible_candidates {};
    automationtest::utilities::Rectangle target {};

    void LoopAllTree(const std::function<void(const LayoutDefinition&)>& action) const;
    std::pair<std::unordered_set<std::string>, std::unordered_set<std::string>> GetAllRegexes() const;
    void LoopLayoutMatching(
        const std::vector<automationtest::utilities::Rectangle>& rectangles,
        const std::function<bool(const LayoutDefinition&)>& predicate);

private:
    void GetAllRegexes(
        std::unordered_set<std::string>& regexes,
        std::unordered_set<std::string>& locations) const;
};

} // namespace automationtest::utilities::status

#endif // AUTOMATIOTEST_STATUS_LAYOUTDEFINITION_HPP
