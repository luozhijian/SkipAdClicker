#include "LayoutDefinition.hpp"

namespace automationtest::utilities::status {

void LayoutDefinition::LoopAllTree(const std::function<void(const LayoutDefinition&)>& action) const
{
    action(*this);
    for (const auto& layout : layouts) {
        layout.LoopAllTree(action);
    }
}

std::pair<std::unordered_set<std::string>, std::unordered_set<std::string>> LayoutDefinition::GetAllRegexes() const
{
    std::unordered_set<std::string> regexes;
    std::unordered_set<std::string> locations;
    GetAllRegexes(regexes, locations);
    return {regexes, locations};
}

void LayoutDefinition::GetAllRegexes(
    std::unordered_set<std::string>& regexes,
    std::unordered_set<std::string>& locations) const
{
    if (!regex.empty()) {
        regexes.insert(regex);
        if (!location.empty()) {
            locations.insert(location);
        }
    }

    for (const auto& layout : layouts) {
        layout.GetAllRegexes(regexes, locations);
    }
}

void LayoutDefinition::LoopLayoutMatching(
    const std::vector<automationtest::utilities::Rectangle>& rectangles,
    const std::function<bool(const LayoutDefinition&)>& predicate)
{
    if (rectangles.empty()) {
        return;
    }

    if (predicate(*this)) {
        possible_candidates.insert(possible_candidates.end(), rectangles.begin(), rectangles.end());
    }

    for (auto& layout : layouts) {
        layout.LoopLayoutMatching(rectangles, predicate);
    }
}

} // namespace automationtest::utilities::status
