#include "RecreationDateTimeClicker.hpp"

#include "../Layout/LayoutDetector.hpp"

namespace automationtest::userinterfacelib {

std::optional<std::tuple<Rectangle, Rectangle>> RecreationDateTimeClicker::FindOutTwoDates(const Bitmap& bitmap, const std::chrono::year_month_day& start, const std::chrono::year_month_day& end, const TextProvider& provider) const
{
    std::optional<Rectangle> start_rect {};
    std::optional<Rectangle> end_rect {};
    const auto start_day = std::to_string(static_cast<unsigned>(start.day()));
    const auto end_day = std::to_string(static_cast<unsigned>(end.day()));

    for (const auto& item : LayoutDetector::FindTextWithSpecificLayout(bitmap, 7, 3, provider)) {
        if (!start_rect.has_value() && item.text == start_day) {
            start_rect = item.rectangle;
        }
        if (!end_rect.has_value() && item.text == end_day) {
            end_rect = item.rectangle;
        }
    }

    if (!start_rect.has_value() || !end_rect.has_value()) {
        return std::nullopt;
    }

    return std::make_tuple(start_rect.value(), end_rect.value());
}

} // namespace automationtest::userinterfacelib
