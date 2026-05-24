#ifndef AUTOMATIOTEST_REGEXLIB_HPP
#define AUTOMATIOTEST_REGEXLIB_HPP

#include <optional>
#include <regex>
#include <string>
#include <vector>

namespace automationtest::utilities {

struct RegexPattern {
    std::string original_pattern {};
    std::string compiled_pattern {};
    std::vector<std::string> capture_names {};
    std::regex compiled {};
};

class RegexLib {
public:
    static std::optional<RegexPattern> AsRegex(const std::string& text);
    static std::optional<RegexPattern> AsRegexCaseSensitive(const std::string& text);
    static std::string MakeStringRegexStyle(const std::string& value);
    static std::string FindRegexInString1(const std::string& input, const RegexPattern& regex, const std::string& name1);
    static std::vector<std::string> FindRegexInString(
        const std::string& input,
        const RegexPattern& regex,
        const std::string& name1,
        const std::optional<std::string>& name2 = std::nullopt,
        const std::optional<std::string>& name3 = std::nullopt,
        const std::optional<std::string>& name4 = std::nullopt,
        const std::optional<std::string>& name5 = std::nullopt);
    static int EstimateWidthOfRegex(const RegexPattern& regex);
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_REGEXLIB_HPP
