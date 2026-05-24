#include "TestParameterHelper.hpp"

#include "../../Utilities/DependencyInjection/DependencyStore.hpp"
#include "../../Utilities/Exceptions/TestException.hpp"
#include "../../Utilities/Services/VariableService.hpp"
#include "../../Utilities/Status/LoadFunctions.hpp"

namespace automationtest::playtestbook::services {

bool TestParameterHelper::ProcessSpecialParameter(const std::string& parameter, std::any& object, testbooklib::flowcontrols::DefFunction* current_function)
{
    object.reset();
    if (parameter.rfind("Special_", 0) == 0) {
        object = GenerateSpecialParameter(parameter.substr(8), current_function);
        return true;
    }
    if (!parameter.empty() && parameter.front() == '$') {
        object = GenerateSpecialParameter(parameter, current_function);
        return true;
    }
    return false;
}

std::any TestParameterHelper::GenerateSpecialParameter(const std::string& special, testbooklib::flowcontrols::DefFunction* current_function)
{
    if (utilities::status::CaseInsensitiveEqual {}(special, "null")) {
        return {};
    }

    if (current_function != nullptr) {
        std::any function_value {};
        if (current_function->Parameters().TryGetValue(special, function_value)) {
            return function_value;
        }
    }

    std::any value {};
    if (utilities::services::VariableService::Instance().TryGetValue(special, value)) {
        return value;
    }

    const auto property = utilities::status::LoadFunctions::Instance().GetProperty(special);
    if (!property.has_value()) {
        throw utilities::exceptions::TestException("Cannot find parameter " + special);
    }

    return property->getter ? property->getter() : std::any {};
}

} // namespace automationtest::playtestbook::services
