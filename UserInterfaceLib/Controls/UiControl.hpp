#ifndef AUTOMATIOTEST_USERINTERFACELIB_CONTROLS_UICONTROL_HPP
#define AUTOMATIOTEST_USERINTERFACELIB_CONTROLS_UICONTROL_HPP

#include "../Actions/UiAction.hpp"
#include "../../Utilities/Common.hpp"
#include "UiDescription.hpp"

#include <memory>
#include <vector>

namespace automationtest::userinterfacelib {

class UiControl {
public:
    UiControl() = default;
    UiControl(const UiControl&) = delete;
    UiControl& operator=(const UiControl&) = delete;
    UiControl(UiControl&&) noexcept = default;
    UiControl& operator=(UiControl&&) noexcept = default;
    virtual ~UiControl() = default;

    [[nodiscard]] UiControl* FindFinalParent() noexcept;
    [[nodiscard]] const UiControl* FindFinalParent() const noexcept;
    virtual void PerformAction(const UiAction& action);
    [[nodiscard]] virtual UiControl* FindControl(ControlType control_type, const std::string& caption);
    [[nodiscard]] Rectangle AbsoluteRectangle() const noexcept;

    UiControl* parent {nullptr};
    Point relative_position {};
    Size size {};
    UiDescription ui_description {};
    std::vector<std::unique_ptr<UiControl>> children {};
    ControlType control_type {ControlType::None};
};

} // namespace automationtest::userinterfacelib

#endif // AUTOMATIOTEST_USERINTERFACELIB_CONTROLS_UICONTROL_HPP
