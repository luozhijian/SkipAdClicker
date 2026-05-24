#include "UiBitmap.hpp"

#include "../TextBlockDetector.hpp"

namespace automationtest::userinterfacelib {

UiBitmap::UiBitmap(Bitmap source_bitmap)
    : bitmap(std::move(source_bitmap))
{
    parent = nullptr;
    control_type = ControlType::Bitmap;
    size = Size {bitmap.width, bitmap.height};
}

void UiBitmap::ParseBitmap()
{
    children.clear();
    for (const auto& rectangle : TextBlockDetector::FindTextBlocks(bitmap)) {
        auto child = std::make_unique<UiControl>();
        child->parent = this;
        child->control_type = ControlType::Pane;
        child->relative_position = Point {rectangle.x, rectangle.y};
        child->size = Size {rectangle.width, rectangle.height};
        children.push_back(std::move(child));
    }
}

UiControl* UiBitmap::FindControl(ControlType desired_control_type, const std::string& caption)
{
    if (children.empty()) {
        ParseBitmap();
    }
    return UiControl::FindControl(desired_control_type, caption);
}

void UiBitmap::PerformAction(ControlType desired_control_type, const std::string& caption, const UiAction& action)
{
    if (auto* control = FindControl(desired_control_type, caption); control != nullptr) {
        control->PerformAction(action);
    }
}

} // namespace automationtest::userinterfacelib
