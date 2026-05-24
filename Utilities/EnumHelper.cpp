#include "EnumHelper.hpp"

#include <cctype>

namespace automationtest::utilities {

types::EnumRelativeLocation EnumHelper::ToEnumRelativeLocation(const std::string& value)
{
    const auto result = TryParseAsEnumWithDefault<types::EnumRelativeLocation>(value, types::EnumRelativeLocation::None);
    if (result == types::EnumRelativeLocation::None && !EqualsIgnoreCase(value, "None")) {
        throw std::runtime_error("Cannot convert to EnumRelativeLocation: " + value);
    }

    return result;
}

bool EnumHelper::EqualsIgnoreCase(std::string_view left, std::string_view right)
{
    if (left.size() != right.size()) {
        return false;
    }

    for (std::size_t index = 0; index < left.size(); ++index) {
        const auto left_char = static_cast<unsigned char>(left[index]);
        const auto right_char = static_cast<unsigned char>(right[index]);
        if (std::tolower(left_char) != std::tolower(right_char)) {
            return false;
        }
    }

    return true;
}

} // namespace automationtest::utilities
