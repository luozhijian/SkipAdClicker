#ifndef AUTOMATIOTEST_APP_FULLSCREENCONTROLS_HPP
#define AUTOMATIOTEST_APP_FULLSCREENCONTROLS_HPP

#include "../Utilities/CommonTypes.hpp"
#include "../Utilities/Status/LoadFunctions.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace automationtest::app {

class FullScreenControls {
public:
    using ScreenshotList = std::vector<std::pair<utilities::Bitmap, utilities::Point>>;

    static std::shared_ptr<ScreenshotList> TakeFullScreenshot();
    static void RegisterBindings(utilities::status::LoadFunctions& load_functions);
};

} // namespace automationtest::app

#endif // AUTOMATIOTEST_APP_FULLSCREENCONTROLS_HPP
