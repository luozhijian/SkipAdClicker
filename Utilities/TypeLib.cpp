#include "TypeLib.hpp"

#include <cctype>
#include <iomanip>
#include <sstream>

namespace automationtest::utilities {

bool TypeLib::HasOtherLetter(const std::string& value)
{
    for (unsigned char ch : value) {
        if (ch > 127) {
            return true;
        }
    }
    return false;
}

std::optional<int> TypeLib::ToNullInt(const std::string& source_value)
{
    if (source_value.empty()) {
        return std::nullopt;
    }
    try {
        return std::stoi(source_value);
    } catch (...) {
        return std::nullopt;
    }
}

std::optional<long long> TypeLib::ToNullLong(const std::string& source_value)
{
    if (source_value.empty()) {
        return std::nullopt;
    }
    try {
        return std::stoll(source_value);
    } catch (...) {
        return std::nullopt;
    }
}

std::optional<double> TypeLib::ToNullDouble(const std::string& source_value)
{
    if (source_value.empty()) {
        return std::nullopt;
    }
    try {
        return std::stod(source_value);
    } catch (...) {
        return std::nullopt;
    }
}

double TypeLib::ToDoubleWithDefault(const std::string& source_value, double default_value)
{
    return ToNullDouble(source_value).value_or(default_value);
}

std::optional<unsigned char> TypeLib::ToNullByte(const std::string& source_value)
{
    if (source_value.empty()) {
        return std::nullopt;
    }
    try {
        const int value = std::stoi(source_value);
        if (value < 0 || value > 255) {
            return std::nullopt;
        }
        return static_cast<unsigned char>(value);
    } catch (...) {
        return std::nullopt;
    }
}

std::optional<bool> TypeLib::ToNullBool(const std::string& source_value)
{
    if (source_value.empty()) {
        return std::nullopt;
    }
    if (source_value == "1") {
        return true;
    }
    if (source_value == "0") {
        return false;
    }

    std::string lower = source_value;
    for (auto& ch : lower) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    if (lower == "true") {
        return true;
    }
    if (lower == "false") {
        return false;
    }
    return std::nullopt;
}

bool TypeLib::ToBoolDefaultFalse(const std::string& source_value)
{
    return ToNullBool(source_value).value_or(false);
}

bool TypeLib::ToBoolDefaultTrue(const std::string& source_value)
{
    return ToNullBool(source_value).value_or(true);
}

bool TypeLib::IsDate(const std::string& value)
{
    if (value.empty()) {
        return false;
    }

    std::tm time {};
    std::istringstream stream(value);
    stream >> std::get_time(&time, "%Y-%m-%d");
    return !stream.fail();
}

} // namespace automationtest::utilities
