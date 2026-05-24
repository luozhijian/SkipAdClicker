#include "UiControl.hpp"

namespace automationtest::userinterfacelib {

UiControl* UiControl::FindFinalParent() noexcept
{
    return parent == nullptr ? this : parent->FindFinalParent();
}

const UiControl* UiControl::FindFinalParent() const noexcept
{
    return parent == nullptr ? this : parent->FindFinalParent();
}

void UiControl::PerformAction(const UiAction& action)
{
    (void)action;
}

UiControl* UiControl::FindControl(ControlType desired_control_type, const std::string& caption)
{
    const auto type_match = desired_control_type == ControlType::None || control_type == desired_control_type;
    const auto caption_match = caption.empty() || ui_description.caption == caption;
    if (type_match && caption_match) {
        return this;
    }

    for (auto& child : children) {
        if (auto* result = child->FindControl(desired_control_type, caption); result != nullptr) {
            return result;
        }
    }

    return nullptr;
}

Rectangle UiControl::AbsoluteRectangle() const noexcept
{
    Point location = relative_position;
    for (auto current = parent; current != nullptr; current = current->parent) {
        location.x += current->relative_position.x;
        location.y += current->relative_position.y;
    }

    return Rectangle {location.x, location.y, size.width, size.height};
}

} // namespace automationtest::userinterfacelib
