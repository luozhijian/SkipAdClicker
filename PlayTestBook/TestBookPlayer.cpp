#include "TestBookPlayer.hpp"

#include "Services/RecentActivityService.hpp"
#include "Services/StaticFunctionService.hpp"
#include "Services/TestParameterHelper.hpp"

#include "../TestBookLib/Action/BlockAction.hpp"
#include "../Utilities/Exceptions/TestCancellingException.hpp"
#include "../Utilities/Exceptions/TestException.hpp"
#include "../Utilities/GlobalSetting.hpp"
#include "../Utilities/GlobalVariables.hpp"
#include "../Utilities/Services/VariableService.hpp"
#include "../Utilities/Logger.hpp"
#include "../Utilities/DisposableStopWatch.hpp"

namespace automationtest::playtestbook {

using automationtest::utilities::exceptions::TestCancellingException;
using automationtest::utilities::exceptions::TestException;
using automationtest::utilities::Logger;
using automationtest::utilities::DisposableStopWatch;

namespace {

std::shared_ptr<utilities::interface::IRecentActionService> CreateRecentActionServiceReference()
{
    return std::shared_ptr<utilities::interface::IRecentActionService>(
        &services::RecentActivityService::Instance(),
        [](utilities::interface::IRecentActionService*) {});
}

} // namespace

TestBookPlayer::TestBookPlayer(std::shared_ptr<testbooklib::TestBook> test_book)
    : test_book_(std::move(test_book))
{
}

void TestBookPlayer::Assign()
{
    testbooklib::SetVariableService(&utilities::services::VariableService::Instance());
    utilities::GlobalVariables::recent_action_service = CreateRecentActionServiceReference();
    services::StaticFunctionService::Instance().RegisterDefaultFunctions();
}

void TestBookPlayer::Play(const std::function<bool()>& cancellation_requested)
{
    Assign();
    cancellation_requested_ = cancellation_requested;

    if (!test_book_) {
        return;
    }

    for (const auto& page : test_book_->Pages()) {
        if (!page) {
            continue;
        }
        page->PlayActions([this](testbooklib::TestAction& action) {
            PlayOneAction(action);
        });
    }
}

const std::shared_ptr<testbooklib::TestBook>& TestBookPlayer::Book() const noexcept
{
    return test_book_;
}

void TestBookPlayer::CheckIsCancelled() const
{
    if (utilities::GlobalSetting::is_stop_test_requested) {
        throw TestCancellingException();
    }
    if (cancellation_requested_ && cancellation_requested_()) {
        throw TestCancellingException();
    }
}

void TestBookPlayer::PlayOneAction(testbooklib::TestAction& action)
{
    CheckIsCancelled();

    if (auto* block_action = dynamic_cast<testbooklib::action::BlockAction*>(&action)) {
        block_action->PlayActions([this](testbooklib::TestAction& child_action) {
            PlayOneAction(child_action);
        });
        return;
    }

    PlayOneActionWithoutBlock(action);
}

void TestBookPlayer::PlayOneActionWithoutBlock(testbooklib::TestAction& action)
{
    CheckIsCancelled();

    DisposableStopWatch watch( Logger::InfoMessage,
        "PlayOneAction: " + action.ToString());

    auto* current_function = self_defined_functions_.empty() ? nullptr : self_defined_functions_.back();
    if (utilities::status::CaseInsensitiveEqual {}(action.action, "let")) {
        ProcessLetAction(action, current_function);
        return;
    }

    if (auto* self_defined_function = utilities::status::LoadFunctions::Instance().GetSelfDefinedFunction(action.action)) {
        ProcessSelfDefinedFunctions(action, *self_defined_function);
        return;
    }

    const auto method = utilities::status::LoadFunctions::Instance().GetMethod(action.action, action.parameters.size());
    if (!method.has_value() || !method->invoke) {
        throw TestException("Cannot find " + action.action);
    }

    try {
        auto parameters = GenerateParameterList(action, method.value(), current_function);
        auto result = method->invoke(parameters);

        if (!action.return_value.empty()) {
            bool already_added_result = false;
            if (current_function != nullptr) {
                already_added_result = current_function->AddResultIfInRange(action.return_value, result);
            }
            if (!already_added_result) {
                utilities::services::VariableService::Instance().SetObject(action.return_value, result);
            }
        } else if (result.has_value() && result.type() == typeid(utilities::Bitmap)) {
            services::RecentActivityService::Instance().PushBitmap(std::any_cast<utilities::Bitmap>(result));
        }
    } catch (const TestException&) {
        throw;
    } catch (const std::exception& exception) {
        throw TestException("When process " + action.action_text + "\n" + exception.what());
    }
}

void TestBookPlayer::ProcessSelfDefinedFunctions(testbooklib::TestAction& action, testbooklib::flowcontrols::DefFunction& self_defined_function)
{
    try {
        auto* previous_function = self_defined_functions_.empty() ? nullptr : self_defined_functions_.back();
        GenerateParameterListForSelfDefinedFunction(action, self_defined_function, previous_function);

        self_defined_functions_.push_back(&self_defined_function);
        try {
            self_defined_function.PlayActions([this](testbooklib::TestAction& child_action) {
                PlayOneAction(child_action);
            });
        } catch (...) {
            self_defined_functions_.pop_back();
            throw;
        }
        self_defined_functions_.pop_back();
    } catch (const TestException&) {
        throw;
    } catch (const std::exception& exception) {
        throw TestException("When ProcessSelfDefinedFunctions " + action.ToString() + "\n" + exception.what());
    }
}

void TestBookPlayer::GenerateParameterListForSelfDefinedFunction(
    testbooklib::TestAction& action,
    testbooklib::flowcontrols::DefFunction& self_defined_function,
    testbooklib::flowcontrols::DefFunction* previous_function)
{
    const auto& target_parameters = self_defined_function.Parameters().GetParameterNames();
    if (action.parameters.size() < target_parameters.size()) {
        throw TestException("input parameter for " + action.action_text + " missing input " + action.ToString());
    }

    for (std::size_t index = 0; index < target_parameters.size(); ++index) {
        std::any value {};
        if (!services::TestParameterHelper::ProcessSpecialParameter(action.parameters[index].Value(), value, previous_function)) {
            value = action.parameters[index].Value();
        }
        self_defined_function.Parameters().AddValue(target_parameters[index], value);
    }
}

std::vector<std::any> TestBookPlayer::GenerateParameterList(
    testbooklib::TestAction& action,
    const RegisteredMethod& method,
    testbooklib::flowcontrols::DefFunction* current_function)
{
    std::vector<std::any> parameters {};
    parameters.reserve(method.parameters.size());

    std::size_t input_index = 0;
    for (const auto& target_parameter : method.parameters) {
        if (target_parameter.is_out) {
            parameters.push_back({});
            continue;
        }

        if (input_index >= action.parameters.size()) {
            if (target_parameter.has_default_value) {
                parameters.push_back(target_parameter.default_value);
                continue;
            }
            throw TestException("input parameter for " + action.action_text + " missing: " + target_parameter.name);
        }

        parameters.push_back(GenerateParameter(action.parameters[input_index], target_parameter, current_function));
        ++input_index;
    }

    return parameters;
}

std::any TestBookPlayer::GenerateParameter(
    const testbooklib::parameter::TestActionParameter& parameter,
    const utilities::status::LoadFunctions::RegisteredParameter& target_parameter,
    testbooklib::flowcontrols::DefFunction* current_function)
{
    if (target_parameter.is_out) {
        return {};
    }

    std::any value {};
    if (!services::TestParameterHelper::ProcessSpecialParameter(parameter.Value(), value, current_function)) {
        value = parameter.Value();
    }

    if (!value.has_value()) {
        return {};
    }
    if (target_parameter.converter) {
        if (value.type() == typeid(std::string)) {
            return target_parameter.converter(std::any_cast<const std::string&>(value));
        }
        return value;
    }
    return value;
}

void TestBookPlayer::ProcessLetAction(testbooklib::TestAction& action, testbooklib::flowcontrols::DefFunction* current_function)
{
    if (action.parameters.empty()) {
        throw TestException("let action requires one input parameter");
    }

    std::any value {};
    if (!services::TestParameterHelper::ProcessSpecialParameter(action.parameters.front().Value(), value, current_function)) {
        value = action.parameters.front().Value();
    }

    if (action.return_value.empty()) {
        throw TestException("let action requires a return value target");
    }

    utilities::services::VariableService::Instance().SetObject(action.return_value, value);
}

} // namespace automationtest::playtestbook
