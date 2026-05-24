#ifndef AUTOMATIOTEST_USERINTERFACELIB_SCROLLAREADETECTOR_HPP
#define AUTOMATIOTEST_USERINTERFACELIB_SCROLLAREADETECTOR_HPP

#include "../Utilities/Common.hpp"
#include "../Utilities/Types/EnumRelativeLocation.hpp"

#include <regex>

namespace automationtest::userinterfacelib {

class ScrollAreaDetector {
public:
    bool ClickMenuInPopup(const Bitmap& bitmap, const Rectangle& popup_rectangle, const std::regex& regex, const ClickCallback& mouse_click, const TextProvider& provider = {});
    std::optional<Bitmap> ClickOnTextWithScrollBarWithTextNearBy(const Bitmap& bitmap, const Rectangle& popup_rectangle, automationtest::utilities::types::EnumRelativeLocation relative_location, const std::regex& regex_for_text, const std::regex& regex_for_click, const ClickCallback& mouse_click, const TextProvider& provider = {}, int wait_milliseconds = 1000);
    static std::optional<Bitmap> ScrolldownClick(const Bitmap& popup_bitmap, int click_times, const ClickCallback& click_callback);
};

} // namespace automationtest::userinterfacelib

#endif // AUTOMATIOTEST_USERINTERFACELIB_SCROLLAREADETECTOR_HPP
