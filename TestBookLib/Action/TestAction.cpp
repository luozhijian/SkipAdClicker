#include "TestAction.hpp"

namespace automationtest::testbooklib {

TestAction::TestAction(int line_number_value, std::string text)
    : action_text(std::move(text)),
      line_number(line_number_value),
      variable_service_(GlobalVariableService())
{
}

std::string TestAction::ToString() const
{
    return action_text;
}

std::any TestAction::GetVariable(const std::string& string_variable) const
{
    if (!string_variable.empty() && string_variable.front() == '$') {
        if (auto* service = GetVariableService()) {
            return service->SolveVariable(string_variable);
        }
        return {};
    }

    return string_variable;
}

std::any TestAction::GetVariable(const std::vector<std::string>& list, int which) const
{
    if (which < 0 || which >= static_cast<int>(list.size())) {
        return {};
    }

    return GetVariable(list[which]);
}

VariableService* TestAction::GetVariableService() const noexcept
{
    if (variable_service_ == nullptr) {
        variable_service_ = GlobalVariableService();
    }

    return variable_service_;
}

} // namespace automationtest::testbooklib
