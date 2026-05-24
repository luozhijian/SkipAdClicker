#include "DateTimeLib.hpp"

#include <array>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace automationtest::utilities {

namespace {

std::tm ToLocalTm(std::chrono::system_clock::time_point value)
{
    const std::time_t time = std::chrono::system_clock::to_time_t(value);
    std::tm local {};
    localtime_s(&local, &time);
    return local;
}

}

std::string DateTimeLib::GetCurrentDateTime_yyyyMMdd_HHmmss()
{
    const auto now = std::chrono::system_clock::now();
    const auto tm = ToLocalTm(now);
    std::ostringstream stream;
    stream << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return stream.str();
}

std::string DateTimeLib::GetCurrentDateTime_yyyyMMdd()
{
    const auto now = std::chrono::system_clock::now();
    const auto tm = ToLocalTm(now);
    std::ostringstream stream;
    stream << std::put_time(&tm, "%Y%m%d");
    return stream.str();
}

std::string DateTimeLib::GetCurrentDateTime_yyyyMMdd_HHmmss_Underscore_atBegining()
{
    return "_" + GetCurrentDateTime_yyyyMMdd_HHmmss();
}

int DateTimeLib::ConvertToStringMonthToInt(const std::string& month)
{
    static const std::array<std::string, 12> months {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };
    for (std::size_t index = 0; index < months.size(); ++index) {
        if (_stricmp(months[index].c_str(), month.c_str()) == 0) {
            return static_cast<int>(index) + 1;
        }
    }
    throw std::runtime_error("Cannot parse month " + month + " to int");
}

bool DateTimeLib::IsSameMonth(const std::chrono::system_clock::time_point& dt1, const std::chrono::system_clock::time_point& dt2)
{
    const auto tm1 = ToLocalTm(dt1);
    const auto tm2 = ToLocalTm(dt2);
    return tm1.tm_year == tm2.tm_year && tm1.tm_mon == tm2.tm_mon;
}

std::chrono::system_clock::time_point DateTimeLib::GetPreviousWeekday(std::chrono::system_clock::time_point date)
{
    static const std::array<std::pair<int, int>, 4> holidays {
        std::pair {12, 25},
        std::pair {1, 1},
        std::pair {1, 20},
        std::pair {2, 17}
    };

    while (true) {
        date -= std::chrono::hours(24);
        const auto tm = ToLocalTm(date);
        const bool weekend = tm.tm_wday == 0 || tm.tm_wday == 6;
        bool holiday = false;
        for (const auto& [month, day] : holidays) {
            if (tm.tm_mon + 1 == month && tm.tm_mday == day) {
                holiday = true;
                break;
            }
        }
        if (!weekend && !holiday) {
            return date;
        }
    }
}

} // namespace automationtest::utilities
