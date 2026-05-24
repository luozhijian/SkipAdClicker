#include "ExpressionHelper.hpp"

#include <regex>

namespace automationtest::utilities::expression {

std::string ExpressionHelper::StringConcatenate(const std::vector<std::optional<std::string>>& values)
{
    std::string result;
    for (const auto& value : values) {
        if (value.has_value()) {
            result += *value;
        }
    }
    return result;
}

std::optional<std::string> ExpressionHelper::SubString(const std::optional<std::string>& value, int start_index, int length)
{
    if (!value.has_value()) {
        return std::nullopt;
    }
    if (length <= 0) {
        return std::string {};
    }
    int adjusted = start_index;
    if (adjusted < 0 && !value->empty()) {
        adjusted = adjusted % static_cast<int>(value->size()) + static_cast<int>(value->size());
    }
    if (adjusted < 0 || adjusted >= static_cast<int>(value->size())) {
        return std::string {};
    }
    if (adjusted + length <= static_cast<int>(value->size())) {
        return value->substr(static_cast<std::size_t>(adjusted), static_cast<std::size_t>(length));
    }
    return value->substr(static_cast<std::size_t>(adjusted));
}

std::optional<std::string> ExpressionHelper::StringReplaceIgnoreCase(const std::optional<std::string>& value, const std::string& search_text, const std::string& replace_text)
{
    if (!value.has_value()) {
        return std::nullopt;
    }
    return std::regex_replace(*value, std::regex(search_text, std::regex_constants::icase), replace_text);
}

double ExpressionHelper::AddNumber(double d1, double d2, std::optional<double> d3) noexcept
{
    return d1 + d2 + d3.value_or(0.0);
}

int ExpressionHelper::AddInteger(int d1, int d2, std::optional<int> d3) noexcept
{
    return d1 + d2 + d3.value_or(0);
}

double ExpressionHelper::TimeNumber(double d1, double d2, std::optional<double> d3) noexcept
{
    return d3.has_value() ? d1 * d2 * *d3 : d1 + d2;
}

} // namespace automationtest::utilities::expression
