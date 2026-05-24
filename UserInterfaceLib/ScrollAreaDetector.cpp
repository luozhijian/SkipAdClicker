#include "ScrollAreaDetector.hpp"

#include "TextBlockDetector.hpp"
#include "../OpencvLib/OpenCvLib.hpp"

namespace automationtest::userinterfacelib {

bool ScrollAreaDetector::ClickMenuInPopup(const Bitmap& bitmap, const Rectangle& popup_rectangle, const std::regex& regex, const ClickCallback& mouse_click, const TextProvider& provider)
{
    return TextBlockDetector::ClickOnTextRandom(bitmap, popup_rectangle, regex, mouse_click, provider).has_value();
}

std::optional<Bitmap> ScrollAreaDetector::ClickOnTextWithScrollBarWithTextNearBy(const Bitmap& bitmap, const Rectangle& popup_rectangle, automationtest::utilities::types::EnumRelativeLocation relative_location, const std::regex& regex_for_text, const std::regex& regex_for_click, const ClickCallback& mouse_click, const TextProvider& provider, int wait_milliseconds)
{
    return TextBlockDetector::ClickOnTextNearText(bitmap, popup_rectangle, relative_location, regex_for_text, regex_for_click, mouse_click, provider, wait_milliseconds);
}

std::optional<Bitmap> ScrollAreaDetector::ScrolldownClick(const Bitmap& popup_bitmap, int click_times, const ClickCallback& click_callback)
{
    if (!click_callback || click_times <= 0) {
        return std::nullopt;
    }

    const auto click_point = Point {popup_bitmap.width - 4, popup_bitmap.height - 8};
    return click_callback(click_point, click_times);
}

} // namespace automationtest::userinterfacelib
