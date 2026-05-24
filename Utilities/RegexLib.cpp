#include "RegexLib.hpp"

#include <algorithm>
#include <cctype>

namespace automationtest::utilities {

namespace {

RegexPattern BuildRegexPattern(const std::string& text, std::regex_constants::syntax_option_type options)
{
    RegexPattern pattern;
    pattern.original_pattern = text;
    pattern.compiled_pattern.reserve(text.size());

    for (std::size_t index = 0; index < text.size(); ++index) {
        if (text[index] == '\\' && index + 1 < text.size()) {
            pattern.compiled_pattern += text[index];
            pattern.compiled_pattern += text[++index];
            continue;
        }

        if (text[index] == '(' && index + 3 < text.size() && text[index + 1] == '?' && text[index + 2] == '<') {
            const std::size_t name_end = text.find('>', index + 3);
            if (name_end != std::string::npos) {
                pattern.capture_names.push_back(text.substr(index + 3, name_end - (index + 3)));
                pattern.compiled_pattern += '(';
                index = name_end;
                continue;
            }
        }

        pattern.compiled_pattern += text[index];
    }

    pattern.compiled = std::regex(pattern.compiled_pattern, options);
    return pattern;
}

std::optional<std::size_t> ResolveGroupIndex(const RegexPattern& regex, const std::string& name)
{
    if (name.empty()) {
        return std::nullopt;
    }

    const bool is_numeric = std::all_of(name.begin(), name.end(), [](unsigned char c) {
        return std::isdigit(c) != 0;
    });

    if (is_numeric) {
        return static_cast<std::size_t>(std::stoul(name));
    }

    for (std::size_t index = 0; index < regex.capture_names.size(); ++index) {
        if (regex.capture_names[index] == name) {
            return index + 1;
        }
    }

    return std::nullopt;
}

} // namespace

std::optional<RegexPattern> RegexLib::AsRegex(const std::string& text)
{
    if (text.find_first_not_of(" \t\r\n") == std::string::npos) {
        return std::nullopt;
    }

    return BuildRegexPattern(text, std::regex_constants::ECMAScript | std::regex_constants::icase);
}

std::optional<RegexPattern> RegexLib::AsRegexCaseSensitive(const std::string& text)
{
    if (text.find_first_not_of(" \t\r\n") == std::string::npos) {
        return std::nullopt;
    }

    return BuildRegexPattern(text, std::regex_constants::ECMAScript);
}

std::string RegexLib::MakeStringRegexStyle(const std::string& value)
{
    if (value.find_first_not_of(" \t\r\n") == std::string::npos) {
        return value;
    }

    std::string escaped = std::regex_replace(value, std::regex(R"(\\)"), R"(\\)");
    return std::regex_replace(escaped, std::regex(R"(\s+)"), R"(\s*)");
}

std::string RegexLib::FindRegexInString1(const std::string& input, const RegexPattern& regex, const std::string& name1)
{
    return FindRegexInString(input, regex, name1)[0];
}

std::vector<std::string> RegexLib::FindRegexInString(
    const std::string& input,
    const RegexPattern& regex,
    const std::string& name1,
    const std::optional<std::string>& name2,
    const std::optional<std::string>& name3,
    const std::optional<std::string>& name4,
    const std::optional<std::string>& name5)
{
    std::vector<std::string> results(5);
    if (input.empty()) {
        return results;
    }

    std::smatch match;
    if (!std::regex_search(input, match, regex.compiled)) {
        return results;
    }

    const std::vector<std::optional<std::string>> group_names {name1, name2, name3, name4, name5};
    for (std::size_t index = 0; index < group_names.size(); ++index) {
        if (!group_names[index].has_value()) {
            continue;
        }

        const auto group_index = ResolveGroupIndex(regex, *group_names[index]);
        if (group_index.has_value() && *group_index < match.size()) {
            results[index] = match[*group_index].str();
        }
    }

    return results;
}

int RegexLib::EstimateWidthOfRegex(const RegexPattern& regex)
{
    const std::string& pattern = regex.original_pattern;
    if (pattern.empty()) {
        return 0;
    }

    int minimum_length = 0;
    for (std::size_t index = 0; index < pattern.size(); ++index) {
        switch (pattern[index]) {
        case '\\':
            if (index + 1 < pattern.size()) {
                ++index;
            }
            ++minimum_length;
            break;
        case '?':
        case '*':
            break;
        case '+':
            ++minimum_length;
            break;
        case '{': {
            const std::size_t end_brace = pattern.find('}', index);
            if (end_brace != std::string::npos && end_brace > index + 1) {
                const std::string quantifier = pattern.substr(index + 1, end_brace - index - 1);
                const std::size_t comma = quantifier.find(',');
                try {
                    minimum_length += std::stoi(quantifier.substr(0, comma));
                } catch (...) {
                }
                index = end_brace;
            }
            break;
        }
        case '[': {
            const std::size_t end_bracket = pattern.find(']', index);
            if (end_bracket != std::string::npos) {
                ++minimum_length;
                index = end_bracket;
            }
            break;
        }
        case '(':
        case ')':
        case '|':
            break;
        default:
            ++minimum_length;
            break;
        }
    }

    return minimum_length;
}

} // namespace automationtest::utilities
