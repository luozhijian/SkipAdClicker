#ifndef AUTOMATIOTEST_USERINTERFACELIB_DATETIME_DATETIMECLICKER_HPP
#define AUTOMATIOTEST_USERINTERFACELIB_DATETIME_DATETIMECLICKER_HPP

#include "../../Utilities/Common.hpp"

#include <chrono>
#include <optional>
#include <regex>

namespace automationtest::userinterfacelib {

class DateTimeClicker {
public:
    static constexpr const char* RegexPatternYearMonth = R"(\b(Jan(?:uary)?|Feb(?:ruary)?|Mar(?:ch)?|Apr(?:il)?|May|Jun(?:e)?|Jul(?:y)?|Aug(?:ust)?|Sep(?:tember)?|Oct(?:ober)?|Nov(?:ember)?|Dec(?:ember)?)\s*(19[7-9]\d|2\d{3})(?=\D|$))";

    DateTimeClicker(Bitmap bitmap, Bitmap background_bitmap, Rectangle popup_rectangle);

    std::optional<Bitmap> ClickOneDate(const std::chrono::year_month_day& target_date, const ClickCallback& mouse_click, const TextProvider& provider = {}, std::optional<Bitmap> begin_with = std::nullopt);
    std::optional<std::pair<int, int>> GetCurrentMonth(const Bitmap& bitmap, const TextProvider& provider = {}) const;

protected:
    std::optional<Bitmap> ClickOneDay(const Bitmap& bitmap, const std::chrono::year_month_day& target_date, const ClickCallback& mouse_click, const TextProvider& provider) const;

    Bitmap bitmap_ {};
    Bitmap background_bitmap_ {};
    Rectangle popup_rectangle_ {};
};

} // namespace automationtest::userinterfacelib

#endif // AUTOMATIOTEST_USERINTERFACELIB_DATETIME_DATETIMECLICKER_HPP
