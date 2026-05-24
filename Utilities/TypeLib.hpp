#ifndef AUTOMATIOTEST_TYPELIB_HPP
#define AUTOMATIOTEST_TYPELIB_HPP

#include <chrono>
#include <optional>
#include <string>

namespace automationtest::utilities {

class TypeLib {
public:
    static bool HasOtherLetter(const std::string& value);
    static std::optional<int> ToNullInt(const std::string& source_value);
    static std::optional<long long> ToNullLong(const std::string& source_value);
    static std::optional<double> ToNullDouble(const std::string& source_value);
    static double ToDoubleWithDefault(const std::string& source_value, double default_value = 0.0);
    static std::optional<unsigned char> ToNullByte(const std::string& source_value);
    static std::optional<bool> ToNullBool(const std::string& source_value);
    static bool ToBoolDefaultFalse(const std::string& source_value);
    static bool ToBoolDefaultTrue(const std::string& source_value);
    static bool IsDate(const std::string& value);
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_TYPELIB_HPP
