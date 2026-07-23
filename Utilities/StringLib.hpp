#ifndef AUTOMATIOTEST_STRINGLIB_HPP
#define AUTOMATIOTEST_STRINGLIB_HPP

#include <cstdint>
#include <codecvt>
#include <locale>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>
#include <cwctype>

namespace automationtest::utilities {



class StringLib {
public:

    // Works for list or containers of strings, numbers, or any printable type
    template <typename T, typename T2>
    static std::string JoinWith(T& container, const std::string& delimiter) {
        std::ostringstream oss;
        bool isFirst = true;

        // The range-based for loop style you requested
        for (const T2& item : container) {
            if (!isFirst) {
                oss << delimiter; // Adds delimiter BEFORE all items except the first
            }

            if constexpr (std::is_same_v<std::remove_cv_t<T2>, std::wstring>) {
                oss << std::wstring_convert<std::codecvt_utf8<wchar_t>> {}.to_bytes(item);
            }
            else {
                oss << item;
            }
            isFirst = false;      // Changes flag so next items get a delimiter
        }

        return oss.str();
    }

    static std::string Trim(std::string text);
    static std::string ToLower(const std::string& value);
    static std::wstring ToLower(std::wstring value);
    static std::string JoinFragments(const std::vector<std::optional<std::string>>& fragments);
    static int CountOverlaps(const std::string& str1, const std::string& str2);

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
