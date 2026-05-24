#ifndef AUTOMATIOTEST_DATETIMELIB_HPP
#define AUTOMATIOTEST_DATETIMELIB_HPP

#include <chrono>
#include <string>

namespace automationtest::utilities {

class DateTimeLib {
public:
    static std::string GetCurrentDateTime_yyyyMMdd_HHmmss();
    static std::string GetCurrentDateTime_yyyyMMdd();
    static std::string GetCurrentDateTime_yyyyMMdd_HHmmss_Underscore_atBegining();
    static int ConvertToStringMonthToInt(const std::string& month);
    static bool IsSameMonth(const std::chrono::system_clock::time_point& dt1, const std::chrono::system_clock::time_point& dt2);
    static std::chrono::system_clock::time_point GetPreviousWeekday(std::chrono::system_clock::time_point date);
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_DATETIMELIB_HPP
