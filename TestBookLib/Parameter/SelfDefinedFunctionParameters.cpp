#include "SelfDefinedFunctionParameters.hpp"

#include <algorithm>
#include <cctype>

namespace automationtest::testbooklib::parameter {

const std::vector<std::string>& SelfDefinedFunctionParameters::GetParameterNames() const noexcept
{
    return parameter_names_;
}

void SelfDefinedFunctionParameters::InitialAddParameter(const std::string& name)
{
    parameter_names_.push_back(name);
    parameters_[NormalizeName(name)] = {};
}

bool SelfDefinedFunctionParameters::TryGetValue(const std::string& name, std::any& value) const
{
    const auto it = parameters_.find(NormalizeName(name));
    if (it == parameters_.end()) {
        return false;
    }
    value = it->second;
    return true;
}

void SelfDefinedFunctionParameters::AddValue(const std::string& name, std::any value)
{
    parameters_[NormalizeName(name)] = std::move(value);
}

std::string SelfDefinedFunctionParameters::NormalizeName(const std::string& name)
{
    std::string normalized = name;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return normalized;
}

} // namespace automationtest::testbooklib::parameter
