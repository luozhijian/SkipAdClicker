#include "DateTimeClicker.hpp"

#include "../Layout/LayoutDetector.hpp"

#include <array>

namespace automationtest::userinterfacelib {

DateTimeClicker::DateTimeClicker(Bitmap bitmap, Bitmap background_bitmap, Rectangle popup_rectangle)
    : bitmap_(std::move(bitmap)),
      background_bitmap_(std::move(background_bitmap)),
      popup_rectangle_(popup_rectangle)
{
}

std::optional<Bitmap> DateTimeClicker::ClickOneDate(const std::chrono::year_month_day& target_date, const ClickCallback& mouse_click, const TextProvider& provider, std::optional<Bitmap> begin_with)
{
    auto working = begin_with.value_or(bitmap_);
    const auto current_month = GetCurrentMonth(working, provider);
    if (!current_month.has_value()) {
        return std::nullopt;
    }
    if (current_month->first == static_cast<int>(target_date.year()) && current_month->second == static_cast<unsigned>(target_date.month())) {
        return ClickOneDay(working, target_date, mouse_click, provider);
    }
    return std::nullopt;
}

std::optional<std::pair<int, int>> DateTimeClicker::GetCurrentMonth(const Bitmap& bitmap, const TextProvider& provider) const
{
    static const std::array<std::string, 12> months {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    const auto matches = LayoutDetector::FindTextWithSpecificLayout(bitmap, 7, 3, provider);
    std::regex regex(RegexPatternYearMonth, std::regex::icase);
    for (const auto& match : matches) {
        std::smatch capture {};
        if (!std::regex_search(match.text, capture, regex)) {
            continue;
        }
        const auto month_text = capture[1].str();
        const auto year_text = capture[2].str();
        const auto iterator = std::find_if(months.begin(), months.end(), [&month_text](const auto& candidate) {
            return month_text.rfind(candidate, 0) == 0;
        });
        if (iterator == months.end()) {
            continue;
        }
        return std::make_pair(std::stoi(year_text), static_cast<int>(std::distance(months.begin(), iterator) + 1));
    }
    return std::nullopt;
}

std::optional<Bitmap> DateTimeClicker::ClickOneDay(const Bitmap& bitmap, const std::chrono::year_month_day& target_date, const ClickCallback& mouse_click, const TextProvider& provider) const
{
    const auto target_day = std::to_string(static_cast<unsigned>(target_date.day()));
    for (const auto& item : LayoutDetector::FindTextWithSpecificLayout(bitmap, 7, 3, provider)) {
        if (item.text == target_day && mouse_click) {
            auto point = Point {item.rectangle.x + item.rectangle.width / 2 + popup_rectangle_.x, item.rectangle.y + item.rectangle.height / 2 + popup_rectangle_.y};
            return mouse_click(point, 1000);
        }
    }
    return std::nullopt;
}

} // namespace automationtest::userinterfacelib
