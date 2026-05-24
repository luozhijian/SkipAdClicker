#include "IfCondition.hpp"

#include "../../Utilities/Exceptions/TestException.hpp"

namespace automationtest::testbooklib::flowcontrols {

using automationtest::utilities::exceptions::TestException;

IfCondition::IfCondition(int line_number, const std::string& text)
    : action::BlockAction(line_number, text)
{
}

void IfCondition::Parse(const std::vector<std::string>& parts)
{
    if (parts.size() < 2) {
        throw TestException("Format error in line " + std::to_string(line_number) + ": " + action_text);
    }

    is_not_condition_ = ToLowerCopy(parts[1]) == "not";
    const auto index = is_not_condition_ ? 2U : 1U;
    if (parts.size() <= index) {
        throw TestException("Format error in line " + std::to_string(line_number) + ": " + action_text);
    }

    condition_ = parts[index];
}

bool IfCondition::CheckConditionIsTrue() const
{
    if (condition_.empty() || condition_.front() != '$' || variable_service == nullptr) {
        throw TestException("IfCondition condition is not variable " + std::to_string(line_number) + " " + action_text);
    }

    const auto value = variable_service->SolveVariable(condition_);
    return is_not_condition_ != AnyToBool(value).value_or(false);
}

void IfCondition::PlayActions(const ActionRunner& runner)
{
    if (!CheckConditionIsTrue()) {
        return;
    }
    action::BlockAction::PlayActions(runner);
}

} // namespace automationtest::testbooklib::flowcontrols
