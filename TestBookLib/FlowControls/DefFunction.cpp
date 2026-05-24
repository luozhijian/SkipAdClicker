#include "DefFunction.hpp"

#include "../../Utilities/Exceptions/TestCancellingException.hpp"
#include "../../Utilities/Status/LoadFunctions.hpp"

namespace automationtest::testbooklib::flowcontrols {

using automationtest::utilities::exceptions::TestCancellingException;
using automationtest::utilities::status::LoadFunctions;

namespace {

automationtest::utilities::types::EnumOnExcpetionAction ParseOnExceptionAction(const std::string& value)
{
    const auto lowered = ToLowerCopy(value);
    if (lowered == "onerrorresumenext") {
        return automationtest::utilities::types::EnumOnExcpetionAction::OnErrorResumeNext;
    }
    if (lowered == "onerrorcontinueloop") {
        return automationtest::utilities::types::EnumOnExcpetionAction::OnErrorContinueLoop;
    }
    if (lowered == "onerrorexitloop") {
        return automationtest::utilities::types::EnumOnExcpetionAction::OnErrorExitLoop;
    }
    return automationtest::utilities::types::EnumOnExcpetionAction::OnErrorRaiseError;
}

} // namespace

DefFunction::DefFunction(int line_number, const std::string& text)
    : action::BlockAction(line_number, text)
{
}

void DefFunction::Parse(const std::vector<std::string>& parts)
{
    if (parts.size() > 1) {
        name_ = parts[1];
        LoadFunctions::Instance().RegisterSelfDefinedFunction(name_, this);
    }
    if (parts.size() > 2) {
        on_exception_action = ParseOnExceptionAction(parts[2]);
    }
    for (std::size_t index = 3; index < parts.size(); ++index) {
        parameters_.InitialAddParameter("$" + parts[index]);
    }
}

void DefFunction::PlayActions(const ActionRunner& runner)
{
    for (auto& child : ChildActions()) {
        try {
            runner(*child);
        } catch (const TestCancellingException&) {
            throw;
        } catch (...) {
            if (on_exception_action == automationtest::utilities::types::EnumOnExcpetionAction::OnErrorExitLoop) {
                break;
            }
            if (on_exception_action == automationtest::utilities::types::EnumOnExcpetionAction::OnErrorResumeNext) {
                continue;
            }
            throw;
        }
    }
}

const std::string& DefFunction::Name() const noexcept
{
    return name_;
}

bool DefFunction::AddResultIfInRange(const std::string& key, std::any value)
{
    std::any existing {};
    if (!parameters_.TryGetValue(key, existing)) {
        return false;
    }
    parameters_.AddValue(key, std::move(value));
    return true;
}

parameter::SelfDefinedFunctionParameters& DefFunction::Parameters() noexcept
{
    return parameters_;
}

const parameter::SelfDefinedFunctionParameters& DefFunction::Parameters() const noexcept
{
    return parameters_;
}

} // namespace automationtest::testbooklib::flowcontrols
