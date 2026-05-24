#ifndef AUTOMATIOTEST_APP_FULLSCREENCONTROLS_HPP
#define AUTOMATIOTEST_APP_FULLSCREENCONTROLS_HPP

#include "../Utilities/CommonTypes.hpp"
#include "../Utilities/Status/LoadFunctions.hpp"

#include <utility>
#include <vector>

namespace automationtest::app {

class FullScreenControls {
public:
    static std::vector<std::pair<utilities::Bitmap, utilities::Point>> TakeFullScreenshot();
    static void RegisterBindings(utilities::status::LoadFunctions& load_functions);
};

} // namespace automationtest::app

#endif // AUTOMATIOTEST_APP_FULLSCREENCONTROLS_HPP
