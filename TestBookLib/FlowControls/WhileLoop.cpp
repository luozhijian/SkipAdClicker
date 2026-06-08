#include "WhileLoop.hpp"

#include "../../Utilities/Exceptions/TestBreakException.hpp"
#include "../../Utilities/Exceptions/TestCancellingException.hpp"
#include "../../Utilities/Exceptions/TestContinueException.hpp"
#include "../../Utilities/Exceptions/TestException.hpp"
#include "../../Utilities/Types/EnumOnExcpetionAction.hpp"
#include "../../Utilities/ExceptionLib.hpp"
#include "../../Utilities/Logger.hpp"

#include <any>
#include <thread>
#include <format>

namespace automationtest::testbooklib::flowcontrols {

using automationtest::utilities::exceptions::TestBreakException;
using automationtest::utilities::exceptions::TestCancellingException;
using automationtest::utilities::exceptions::TestContinueException;
using automationtest::utilities::exceptions::TestException;
using automationtest::utilities::types::EnumOnExcpetionAction;

namespace {

std::string AnyToString(const std::any& value)
{
    if (!value.has_value()) {
        return {};
    }
    if (value.type() == typeid(std::string)) {
        return std::any_cast<std::string>(value);
    }
    if (value.type() == typeid(const char*)) {
        return std::any_cast<const char*>(value);
    }
    if (value.type() == typeid(bool)) {
        return std::any_cast<bool>(value) ? "true" : "false";
    }
    if (value.type() == typeid(int)) {
        return std::to_string(std::any_cast<int>(value));
    }
    if (value.type() == typeid(double)) {
        return std::to_string(std::any_cast<double>(value));
    }
    if (value.type() == typeid(float)) {
        return std::to_string(std::any_cast<float>(value));
    }
    return {};
}

EnumOnExcpetionAction ParseOnExceptionAction(const std::string& value)
{
    const auto lowered = ToLowerCopy(value);
    if (lowered == "onerrorresumenext") {
        return EnumOnExcpetionAction::OnErrorResumeNext;
    }
    if (lowered == "onerrorcontinueloop") {
        return EnumOnExcpetionAction::OnErrorContinueLoop;
    }
    if (lowered == "onerrorexitloop") {
        return EnumOnExcpetionAction::OnErrorExitLoop;
    }
    if (lowered == "onerrorcounterrorstopatmax") {
        return EnumOnExcpetionAction::OnErrorCountErrorStopAtMax;
    }
    if (lowered == "onerrorcountcontinuouserrorstopatmax") {
        return EnumOnExcpetionAction::OnErrorCountContinuousErrorStopAtMax;
    }
    return EnumOnExcpetionAction::OnErrorRaiseError;
}

} // namespace

WhileLoop::WhileLoop(int line_number, const std::string& text)
    : action::BlockAction(line_number, text)
{
}

void WhileLoop::Parse(const std::vector<std::string>& parts)
{
    if (parts.size() < 2) {
        throw TestException("Format error in line " + std::to_string(line_number) + ": " + action_text);
    }
    raw_parts_ = parts;
}

void WhileLoop::ParseAtRuntime(const std::vector<std::string>& parts)
{
    loop_variable_ = parts[1];
    sleep_seconds_ = parts.size() > 2 ? AnyToDouble(GetVariable(parts, 2)).value_or(-1.0) : -1.0;
    total_seconds_ = parts.size() > 3 ? AnyToDouble(GetVariable(parts, 3)).value_or(-1.0) : -1.0;
    on_exception_action = parts.size() > 4
        ? ParseOnExceptionAction(AnyToString(GetVariable(parts, 4)))
        : EnumOnExcpetionAction::OnErrorRaiseError;
    max_error_count_ = parts.size() > 5 ? AnyToInt(GetVariable(parts, 5)) : std::nullopt;
}

void WhileLoop::PlayActions(const ActionRunner& runner)
{
    if (!first_time_played) {
        first_time_played = true;
        ParseAtRuntime(raw_parts_);
    }

    start_time = std::chrono::system_clock::now();
    const auto end_time = total_seconds_ > 0.0
        ? std::optional(start_time + std::chrono::milliseconds(static_cast<int>(total_seconds_ * 1000.0)))
        : std::optional<std::chrono::system_clock::time_point> {};

    int loop = -1;
    const auto max_count = AnyToInt(std::any(loop_variable_));
    while (true) {
        ++loop;
        if (max_count.has_value() && loop >= max_count.value()) {
            break;
        }
        if (!loop_variable_.empty() && loop_variable_.front() == '$') {
            auto* service = GetVariableService();
            const auto value = service != nullptr ? AnyToBool(service->SolveVariable(loop_variable_)) : std::nullopt;
            if (value.has_value() && !value.value()) {
                break;
            }
        }
        if (end_time.has_value() && std::chrono::system_clock::now() >= end_time.value()) {
            break;
        }

        bool running_experienced_error = false;
        try {
            for (auto& child : ChildActions()) {
                try {
                    runner(*child);
                } catch (const TestBreakException&) {
                    throw;
                } catch (const TestContinueException&) {
                    throw;
                } catch (const TestCancellingException&) {
                    throw;
                } catch (...) {
                    running_experienced_error = true;
                    if (on_exception_action == EnumOnExcpetionAction::OnErrorExitLoop) {
                        throw TestBreakException();
                    }
                    if (on_exception_action == EnumOnExcpetionAction::OnErrorContinueLoop) {
                        throw TestContinueException();
                    }
                    if (on_exception_action != EnumOnExcpetionAction::OnErrorResumeNext) {
                        if (on_exception_action == EnumOnExcpetionAction::OnErrorCountErrorStopAtMax
                            || on_exception_action == EnumOnExcpetionAction::OnErrorCountContinuousErrorStopAtMax) {
                            ++current_error_count_;
                            if (max_error_count_.has_value() && current_error_count_ >= max_error_count_.value()) {
                                std::string message = std::format("Max Error {0} reached: {1}", max_error_count_.value(), automationtest::utilities::ExceptionLib::exception_to_string());
                                automationtest::utilities::Logger::LogToView(message);
                                throw TestBreakException(message);
                            }
                            continue;
                        }
                        throw;
                    }
                }
            }
        } catch (const TestContinueException&) {
        } catch (const TestBreakException&) {
            break;
        }

        if (on_exception_action == EnumOnExcpetionAction::OnErrorCountContinuousErrorStopAtMax && !running_experienced_error) {
            current_error_count_ = 0;
        }

        if (sleep_seconds_ > 0.0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sleep_seconds_ * 1000.0)));
        }
    }
}

} // namespace automationtest::testbooklib::flowcontrols
