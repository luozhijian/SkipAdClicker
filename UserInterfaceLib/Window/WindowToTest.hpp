#ifndef AUTOMATIOTEST_USERINTERFACELIB_WINDOW_WINDOWTOTEST_HPP
#define AUTOMATIOTEST_USERINTERFACELIB_WINDOW_WINDOWTOTEST_HPP

#include "../Actions/UiAction.hpp"
#include "../Controls/UiBitmap.hpp"

#include <functional>
#include <optional>
#include <vector>

namespace automationtest::userinterfacelib {

class WindowToTest {
public:
    using ScreenshotProvider = std::function<std::optional<Bitmap>()>;

    explicit WindowToTest(ScreenshotProvider screenshot_provider = {});

    bool PerformTest(const std::vector<UiAction>& testing_actions);
    std::optional<Bitmap> GetScreenShot() const;
    UiBitmap ParsePage(const Bitmap& bitmap) const;

private:
    ScreenshotProvider screenshot_provider_ {};
};

} // namespace automationtest::userinterfacelib

#endif // AUTOMATIOTEST_USERINTERFACELIB_WINDOW_WINDOWTOTEST_HPP
