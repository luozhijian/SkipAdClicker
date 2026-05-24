#ifndef AUTOMATIOTEST_USERINTERFACELIB_CONTROLS_UIBITMAP_HPP
#define AUTOMATIOTEST_USERINTERFACELIB_CONTROLS_UIBITMAP_HPP

#include "../../Utilities/Common.hpp"
#include "UiControl.hpp"

namespace automationtest::userinterfacelib {

class UiBitmap : public UiControl {
public:
    explicit UiBitmap(Bitmap bitmap);
    UiBitmap(const UiBitmap&) = delete;
    UiBitmap& operator=(const UiBitmap&) = delete;
    UiBitmap(UiBitmap&&) noexcept = default;
    UiBitmap& operator=(UiBitmap&&) noexcept = default;

    void ParseBitmap();
    [[nodiscard]] UiControl* FindControl(ControlType control_type, const std::string& caption) override;
    void PerformAction(ControlType control_type, const std::string& caption, const UiAction& action);

    Bitmap bitmap {};
};

} // namespace automationtest::userinterfacelib

#endif // AUTOMATIOTEST_USERINTERFACELIB_CONTROLS_UIBITMAP_HPP
