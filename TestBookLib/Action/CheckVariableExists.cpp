#include "CheckVariableExists.hpp"

#include "../../Utilities/Exceptions/TestBreakException.hpp"
#include "../../Utilities/Exceptions/TestContinueException.hpp"
#include "../../Utilities/Exceptions/TestException.hpp"

namespace automationtest::testbooklib::action {

using automationtest::utilities::exceptions::TestBreakException;
using automationtest::utilities::exceptions::TestContinueException;
using automationtest::utilities::exceptions::TestException;

CheckVariableExists::CheckVariableExists(int line_number, const std::string& text)
    : BlockAction(line_number, text)
{
}

void CheckVariableExists::Parse(const std::vector<std::string>& parts)
{
    if (parts.size() <= 1) {
        throw TestException("Parameters for CheckVariableExists should be no less than 2");
    }
    variable_name_ = Trim(parts[1]);
    check_if_exists_ = parts.size() > 2 ? AnyToBool(std::any(parts[2])).value_or(true) : true;
    check_if_not_null_ = parts.size() > 4 ? AnyToBool(std::any(parts[4])).value_or(true) : true;
    check_if_not_whitespace_ = parts.size() > 5 ? AnyToBool(std::any(parts[5])).value_or(true) : true;
}

void CheckVariableExists::PlayActions(const ActionRunner& runner)
{
    if (variable_service == nullptr) {
        return;
    }

    const auto exists = variable_service->ContainVariable(variable_name_);
    bool should_run = true;
    if (check_if_exists_) {
        should_run = exists;
        if (should_run && check_if_not_null_) {
            std::any value {};
            should_run = variable_service->TryGetValue("$" + variable_name_, value) && value.has_value();
            if (should_run && check_if_not_whitespace_) {
                should_run = !Trim(value.type() == typeid(std::string) ? std::any_cast<std::string>(value) : std::string {}).empty();
            }
        }
    } else {
        should_run = !exists;
    }

    if (!should_run) {
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
