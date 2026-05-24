#include "WindowToTest.hpp"

namespace automationtest::userinterfacelib {

WindowToTest::WindowToTest(ScreenshotProvider screenshot_provider)
    : screenshot_provider_(std::move(screenshot_provider))
{
}

bool WindowToTest::PerformTest(const std::vector<UiAction>& testing_actions)
{
    const auto bitmap = GetScreenShot();
    if (!bitmap.has_value()) {
        return false;
    }

    auto ui_bitmap = ParsePage(bitmap.value());
    for (const auto& action : testing_actions) {
        if (auto* matched = ui_bitmap.FindControl(action.control_type, action.caption); matched != nullptr) {
            matched->PerformAction(action);
        }
    }
    return true;
}

std::optional<Bitmap> WindowToTest::GetScreenShot() const
{
    return screenshot_provider_ ? screenshot_provider_() : std::nullopt;
}

UiBitmap WindowToTest::ParsePage(const Bitmap& bitmap) const
{
    UiBitmap ui_bitmap(bitmap);
    ui_bitmap.ParseBitmap();
    return ui_bitmap;
}

} // namespace automationtest::userinterfacelib
