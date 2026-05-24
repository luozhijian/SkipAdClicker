#include "DoWhileLoop.hpp"

#include "../../Utilities/Exceptions/TestBreakException.hpp"
#include "../../Utilities/Exceptions/TestContinueException.hpp"
#include "../../Utilities/Exceptions/TestException.hpp"
#include "../../Utilities/Types/EnumOnExcpetionAction.hpp"

#include <thread>

namespace automationtest::testbooklib::flowcontrols {

using automationtest::utilities::exceptions::TestBreakException;
using automationtest::utilities::exceptions::TestContinueException;
using automationtest::utilities::exceptions::TestException;
using automationtest::utilities::types::EnumOnExcpetionAction;

DoWhileLoop::DoWhileLoop(int line_number, const std::string& text)
    : action::BlockAction(line_number, text)
{
}

void DoWhileLoop::Parse(const std::vector<std::string>& parts)
{
    if (parts.size() < 4) {
        throw TestException("Format error in line " + std::to_string(line_number) + ": " + action_text);
    }
    loop_variable_ = parts[1];
    sleep_seconds_ = AnyToDouble(parts[2]).value_or(-1.0);
    total_seconds_ = AnyToDouble(parts[3]).value_or(-1.0);
}

void DoWhileLoop::PlayActions(const ActionRunner& runner)
{
    start_time = std::chrono::system_clock::now();
    const auto end_time = total_seconds_ > 0.0
        ? std::optional(start_time + std::chrono::milliseconds(static_cast<int>(total_seconds_ * 1000.0)))
        : std::optional<std::chrono::system_clock::time_point> {};

    int loop = 0;
    const auto max_count = variable_service ? AnyToInt(variable_service->SolveVariable(loop_variable_)) : std::nullopt;
    while (true) {
        ++loop;
        try {
            for (auto& child : ChildActions()) {
                try {
                    runner(*child);
                } catch (const TestBreakException&) {
                    throw;
                } catch (const TestContinueException&) {
                    throw;
                } catch (...) {
                    if (on_exception_action == EnumOnExcpetionAction::OnErrorExitLoop) {
                        throw TestBreakException();
                    }
                    if (on_exception_action == EnumOnExcpetionAction::OnErrorContinueLoop) {
                        throw TestContinueException();
                    }
                    if (on_exception_action != EnumOnExcpetionAction::OnErrorResumeNext) {
                        throw;
                    }
                }
            }
        } catch (const TestContinueException&) {
        } catch (const TestBreakException&) {
            break;
        }

        if (max_count.has_value() && loop >= max_count.value()) {
            break;
        }
        if (!loop_variable_.empty() && loop_variable_.front() == '$' && variable_service != nullptr) {
            const auto value = AnyToBool(variable_service->SolveVariable(loop_variable_));
            if (value.has_value() && !value.value()) {
                break;
            }
        }
        if (end_time.has_value() && std::chrono::system_clock::now() >= end_time.value()) {
            break;
        }
        if (sleep_seconds_ > 0.0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sleep_seconds_ * 1000.0)));
        }
    }
}

} // namespace automationtest::testbooklib::flowcontrols
