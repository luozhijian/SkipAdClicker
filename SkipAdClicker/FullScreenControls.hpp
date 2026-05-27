#ifndef AUTOMATIOTEST_APP_FULLSCREENCONTROLS_HPP
#define AUTOMATIOTEST_APP_FULLSCREENCONTROLS_HPP

#include "FullScreenCapture.hpp"

#include "../Utilities/Status/LoadFunctions.hpp"

#include <memory>

namespace automationtest::app {

class FullScreenControls {
public:
    using ScreenshotList = FullScreenCapture::ScreenshotList;

    static std::shared_ptr<ScreenshotList> TakeFullScreenshot();
    static void RegisterBindings(utilities::status::LoadFunctions& load_functions);
};

} // namespace automationtest::app

#endif // AUTOMATIOTEST_APP_FULLSCREENCONTROLS_HPP
