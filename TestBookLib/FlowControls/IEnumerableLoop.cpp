#include "IEnumerableLoop.hpp"

#include "../../Utilities/Exceptions/TestBreakException.hpp"
#include "../../Utilities/Exceptions/TestContinueException.hpp"
#include "../../Utilities/Exceptions/TestException.hpp"

namespace automationtest::testbooklib::flowcontrols {

using automationtest::utilities::exceptions::TestBreakException;
using automationtest::utilities::exceptions::TestContinueException;
using automationtest::utilities::exceptions::TestException;

IEnumerableLoop::IEnumerableLoop(int line_number, const std::string& text)
    : action::BlockAction(line_number, text)
{
}

void IEnumerableLoop::Parse(const std::vector<std::string>& parts)
{
    if (parts.size() < 4) {
        throw TestException("IEnumerableLoop should have at least 3 parameters");
    }
    input_variable_ = parts[1];
    loop_variable_ = parts[3];
}

void IEnumerableLoop::SetVariable(const std::any& value)
{
    if (variable_service != nullptr) {
        variable_service->SetObject("$" + loop_variable_, value);
    }
}

void IEnumerableLoop::PlayActions(const ActionRunner& runner)
{
    if (variable_service == nullptr) {
        return;
    }

    const auto value = variable_service->SolveVariable(input_variable_);
    if (value.type() == typeid(std::vector<std::string>)) {
        for (const auto& item : std::any_cast<const std::vector<std::string>&>(value)) {
            SetVariable(item);
            try {
                action::BlockAction::PlayActions(runner);
            } catch (const TestContinueException&) {
            } catch (const TestBreakException&) {
                break;
            }
        }
    }
}

} // namespace automationtest::testbooklib::flowcontrols
