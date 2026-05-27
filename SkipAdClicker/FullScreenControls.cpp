#include "FullScreenControls.hpp"

#include <any>
#include <vector>

namespace automationtest::app {

std::shared_ptr<FullScreenControls::ScreenshotList> FullScreenControls::TakeFullScreenshot()
{
    return FullScreenCapture::TakeFullScreenshot();
}

void FullScreenControls::RegisterBindings(utilities::status::LoadFunctions& load_functions)
{
    load_functions.RegisterMethod("TakeFullScreenshot", utilities::status::LoadFunctions::RegisteredMethod {
        .declaring_type = "FullScreenControls",
        .invoke = [](const std::vector<std::any>&) -> std::any {
            return TakeFullScreenshot();
        },
    });
}

} // namespace automationtest::app
