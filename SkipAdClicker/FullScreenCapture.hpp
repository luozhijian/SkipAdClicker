#ifndef AUTOMATIOTEST_APP_FULLSCREENCAPTURE_HPP
#define AUTOMATIOTEST_APP_FULLSCREENCAPTURE_HPP

#include "../Utilities/CommonTypes.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace automationtest::app {

class FullScreenCapture {
public:
    using ScreenshotList = std::vector<std::pair<utilities::Bitmap, utilities::Point>>;

    static std::shared_ptr<ScreenshotList> TakeFullScreenshot();
};

} // namespace automationtest::app

#endif // AUTOMATIOTEST_APP_FULLSCREENCAPTURE_HPP
