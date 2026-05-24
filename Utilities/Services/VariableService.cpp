#include "VariableService.hpp"

#include "../Exceptions/TestException.hpp"

#include <algorithm>
#include <cctype>

namespace automationtest::utilities::services {

VariableService& VariableService::Instance()
{
    static VariableService instance;
    return instance;
}

bool VariableService::TryGetValue(const std::string& name, std::any& value) const
{
    const auto it = resources_.find(NormalizeName(name));
    if (it == resources_.end()) {
        return false;
    }
    value = it->second;
    return true;
}

void VariableService::SetObject(const std::string& name, std::any value)
{
    resources_[NormalizeName(name)] = std::move(value);
}

std::any VariableService::SolveVariable(const std::any& name) const
{
    if (!name.has_value()) {
        return {};
    }

    if (name.type() == typeid(std::string)) {
        const auto& text = std::any_cast<const std::string&>(name);
        if (!text.empty() && text.front() == '$') {
            return SolveResource(text);
        }
    }

    return name;
}

std::any VariableService::SolveResource(const std::string& name) const
{
    const auto it = resources_.find(NormalizeName(name));
    if (it == resources_.end()) {
        throw exceptions::TestException("Cannot find resource " + name);
    }
    return it->second;
}

bool VariableService::ContainVariable(const std::string& name) const
{
    return resources_.find(NormalizeName("$" + name)) != resources_.end();
}

bool VariableService::RemoveVariable(const std::string& name)
{
    return resources_.erase(NormalizeName(name)) > 0;
}

std::optional<std::any> VariableService::TryGetSettingType(const std::string& type_name) const
{
    return setting_stacks_.GetStackedType(type_name);
}

void VariableService::SaveSetting(const std::string& type_name, std::any value)
{
    if (!setting_stacks_.AddObject(type_name, value)) {
        resources_[NormalizeName(type_name)] = std::move(value);
    }
}

std::string VariableService::NormalizeName(const std::string& name)
{
    std::string normalized = name;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return normalized;
}

} // namespace automationtest::utilities::services
