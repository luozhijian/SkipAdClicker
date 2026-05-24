#include "StringLib.hpp"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>

namespace automationtest::utilities {

namespace {

std::string ToLower(const std::string& value)
{
    std::string result(value);
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return result;
}

std::string JoinFragments(const std::vector<std::optional<std::string>>& fragments)
{
    std::string result;
    for (const auto& fragment : fragments) {
        if (fragment.has_value()) {
            result += *fragment;
        }
    }
    return result;
}

bool HasNoFragments(const std::vector<std::optional<std::string>>& fragments)
{
    return std::none_of(fragments.begin(), fragments.end(), [](const auto& fragment) {
        return fragment.has_value();
    });
}

} // namespace

std::string StringLib::SetStringAsEmpty()
{
    return {};
}

std::string StringLib::StoreString(const std::string& value)
{
    return value;
}

bool StringLib::StringCompareFirstLessThanOrEqualSecond(const std::string& first, const std::string& second) noexcept
{
    return first <= second;
}

bool StringLib::StringCompareFirstLessThanSecond(const std::string& first, const std::string& second) noexcept
{
    return first < second;
}

std::optional<std::string> StringLib::StringLeft(const std::optional<std::string>& value, int length)
{
    if (!value.has_value()) {
        return std::nullopt;
    }

    if (length < 0 || length >= static_cast<int>(value->size())) {
        return value;
    }

    return value->substr(0, static_cast<std::size_t>(length));
}

std::optional<std::string> StringLib::StringRight(const std::optional<std::string>& value, int length)
{
    if (!value.has_value()) {
        return std::nullopt;
    }

    if (length < 0 || length >= static_cast<int>(value->size())) {
        return value;
    }

    return value->substr(value->size() - static_cast<std::size_t>(length), static_cast<std::size_t>(length));
}

bool StringLib::IsEqualIgnoreCase(const std::optional<std::string>& first, const std::optional<std::string>& second) noexcept
{
    if (!first.has_value() && !second.has_value()) {
        return true;
    }
    if (!first.has_value() || !second.has_value()) {
        return false;
    }

    return ToLower(*first) == ToLower(*second);
}

bool StringLib::StartsWithIgnoreCase(const std::optional<std::string>& value, const std::optional<std::string>& prefix) noexcept
{
    if (!value.has_value() && !prefix.has_value()) {
        return true;
    }
    if (!value.has_value() || !prefix.has_value()) {
        return false;
    }
    if (prefix->size() > value->size()) {
        return false;
    }

    return ToLower(value->substr(0, prefix->size())) == ToLower(*prefix);
}

bool StringLib::EndsWithIgnoreCase(const std::optional<std::string>& value, const std::optional<std::string>& suffix) noexcept
{
    if (!value.has_value() && !suffix.has_value()) {
        return true;
    }
    if (!value.has_value() || !suffix.has_value()) {
        return false;
    }
    if (suffix->size() > value->size()) {
        return false;
    }

    return ToLower(value->substr(value->size() - suffix->size())) == ToLower(*suffix);
}

std::string StringLib::ByteArrayToString(const std::vector<std::uint8_t>& bytes)
{
    std::ostringstream builder;
    builder << std::hex << std::setfill('0');
    for (const auto byte : bytes) {
        builder << std::setw(2) << static_cast<int>(byte);
    }
    return builder.str();
}

std::string StringLib::StringAppendLine(const std::string& initial_string, const std::vector<std::optional<std::string>>& fragments)
{
    if (HasNoFragments(fragments)) {
        return initial_string + '\n';
    }

    return initial_string + JoinFragments(fragments) + '\n';
}

std::string StringLib::StringAppend(const std::string& initial_string, const std::vector<std::optional<std::string>>& fragments)
{
    if (HasNoFragments(fragments)) {
        return initial_string;
    }

    return initial_string + JoinFragments(fragments);
}

std::vector<double> StringLib::StringToDoubleList(const std::string& initial_string, double default_value)
{
    std::vector<double> result;
    if (initial_string.find_first_not_of(" \t\r\n") == std::string::npos) {
        return {default_value};
    }

    std::istringstream parser(initial_string);
    std::string part;
    while (parser >> part) {
        try {
            result.push_back(std::stod(part));
        } catch (...) {
        }
    }

    if (result.empty()) {
        return {default_value};
    }

    return result;
}


bool StringLib::StartsWithAfterTrim(const std::string& s, char ch)
{
    size_t i = 0;

    // skip leading whitespace
    while (i < s.size() &&
        std::isspace(static_cast<unsigned char>(s[i])))
    {
        ++i;
    }

    // check if next character is '#'
    return i < s.size() && s[i] == ch;
}

} // namespace automationtest::utilities
