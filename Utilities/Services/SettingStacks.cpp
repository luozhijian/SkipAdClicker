#include "SettingStacks.hpp"

namespace automationtest::utilities::services {

bool SettingStacks::AddStackedType(const std::string& type_name)
{
    const auto [_, inserted] = stacked_types_.try_emplace(type_name, std::vector<std::any> {});
    return !inserted;
}

std::optional<std::any> SettingStacks::GetStackedType(const std::string& type_name) const
{
    const auto it = stacked_types_.find(type_name);
    if (it == stacked_types_.end() || it->second.empty()) {
        return std::nullopt;
    }
    return it->second.back();
}

bool SettingStacks::AddObject(const std::string& type_name, std::any value)
{
    const auto it = stacked_types_.find(type_name);
    if (it == stacked_types_.end()) {
        return false;
    }
    it->second.push_back(std::move(value));
    return true;
}

} // namespace automationtest::utilities::services
