#ifndef AUTOMATIOTEST_STRINGLIB_HPP
#define AUTOMATIOTEST_STRINGLIB_HPP

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace automationtest::utilities {

class StringLib {
public:
    static std::string SetStringAsEmpty();
    static std::string StoreString(const std::string& value);
    static bool StringCompareFirstLessThanOrEqualSecond(const std::string& first, const std::string& second) noexcept;
    static bool StringCompareFirstLessThanSecond(const std::string& first, const std::string& second) noexcept;
    static std::optional<std::string> StringLeft(const std::optional<std::string>& value, int length = -1);
    static std::optional<std::string> StringRight(const std::optional<std::string>& value, int length = -1);
    static bool IsEqualIgnoreCase(const std::optional<std::string>& first, const std::optional<std::string>& second) noexcept;
    static bool StartsWithIgnoreCase(const std::optional<std::string>& value, const std::optional<std::string>& prefix) noexcept;
    static bool EndsWithIgnoreCase(const std::optional<std::string>& value, const std::optional<std::string>& suffix) noexcept;
    static std::string ByteArrayToString(const std::vector<std::uint8_t>& bytes);
    static std::string StringAppendLine(const std::string& initial_string, const std::vector<std::optional<std::string>>& fragments = {});
    static std::string StringAppend(const std::string& initial_string, const std::vector<std::optional<std::string>>& fragments = {});
    static std::vector<double> StringToDoubleList(const std::string& initial_string, double default_value = 1.0);
    static bool StartsWithAfterTrim(const std::string& s, char ch);
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_STRINGLIB_HPP
