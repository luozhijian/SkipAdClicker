#ifndef AUTOMATIOTEST_USERINTERFACELIB_DATETIME_RECREATIONDATETIMECLICKER_HPP
#define AUTOMATIOTEST_USERINTERFACELIB_DATETIME_RECREATIONDATETIMECLICKER_HPP

#include "DateTimeClicker.hpp"

#include <tuple>

namespace automationtest::userinterfacelib {

class RecreationDateTimeClicker : public DateTimeClicker {
public:
    using DateTimeClicker::DateTimeClicker;

    std::optional<std::tuple<Rectangle, Rectangle>> FindOutTwoDates(const Bitmap& bitmap, const std::chrono::year_month_day& start, const std::chrono::year_month_day& end, const TextProvider& provider = {}) const;
};

} // namespace automationtest::userinterfacelib

#endif // AUTOMATIOTEST_USERINTERFACELIB_DATETIME_RECREATIONDATETIMECLICKER_HPP
