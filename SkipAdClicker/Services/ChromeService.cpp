#include "ChromeService.hpp"

#include "../LogView.hpp"

namespace automationtest::app::services {

ChromeService& ChromeService::Instance()
{
    static ChromeService instance;
    return instance;
}

void ChromeService::RegisterBindings(utilities::status::LoadFunctions& load_functions)
{
    using Method = utilities::status::LoadFunctions::RegisteredMethod;
    using Parameter = utilities::status::LoadFunctions::RegisteredParameter;

    load_functions.RegisterMethod("FindCurrentChrome", Method {
        .declaring_type = "ChromeService",
        .invoke = [](const std::vector<std::any>&) -> std::any {
            LogView::AddLog("FindCurrentChrome uses the Qt interactive action fallback in this port.");
            return ChromeService::Instance().TakeScreenShot();
        },
        .parameters = {
            Parameter {.name = "searchString", .converter = [](const std::string& value) -> std::any {
                return value;
            }},
        },
    });
}

} // namespace automationtest::app::services
