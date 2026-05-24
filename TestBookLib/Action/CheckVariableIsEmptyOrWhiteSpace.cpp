#include "CheckVariableIsEmptyOrWhiteSpace.hpp"

#include "../../Utilities/Exceptions/TestBreakException.hpp"
#include "../../Utilities/Exceptions/TestContinueException.hpp"
#include "../../Utilities/Exceptions/TestException.hpp"

namespace automationtest::testbooklib::action {

using automationtest::utilities::exceptions::TestBreakException;
using automationtest::utilities::exceptions::TestContinueException;
using automationtest::utilities::exceptions::TestException;

CheckVariableIsEmptyOrWhiteSpace::CheckVariableIsEmptyOrWhiteSpace(int line_number, const std::string& text)
    : BlockAction(line_number, text)
{
}

void CheckVariableIsEmptyOrWhiteSpace::Parse(const std::vector<std::string>& parts)
{
    if (parts.size() <= 1) {
        throw TestException("Parameters for CheckVariableIsEmptyOrWhiteSpace should be no less than 2");
    }
    variable_name_ = Trim(parts[1]);
}

void CheckVariableIsEmptyOrWhiteSpace::PlayActions(const ActionRunner& runner)
{
    if (variable_service == nullptr) {
        return;
    }

    bool empty_or_whitespace = !variable_service->ContainVariable(variable_name_);
    if (!empty_or_whitespace) {
        std::any value {};
        variable_service->TryGetValue("$" + variable_name_, value);
        const auto text = value.type() == typeid(std::string) ? std::any_cast<std::string>(value) : std::string {};
        empty_or_whitespace = Trim(text).empty();
    }

    if (!empty_or_whitespace) {
        return;
    }

    for (auto& child : ChildActions()) {
        try {
            runner(*child);
        } catch (const TestContinueException&) {
            throw;
        } catch (const TestBreakException&) {
            throw;
        } catch (...) {
            if (on_exception_action != automationtest::utilities::types::EnumOnExcpetionAction::OnErrorResumeNext) {
                throw;
            }
        }
    }
}

} // namespace automationtest::testbooklib::action
