#ifndef AUTOMATIOTEST_TYPES_ENUMSIZEDESCRIPTIONHELPER_HPP
#define AUTOMATIOTEST_TYPES_ENUMSIZEDESCRIPTIONHELPER_HPP

#include "../CommonTypes.hpp"
#include "EnumSizeDescription.hpp"

namespace automationtest::utilities::types {

class EnumSizeDescriptionHelper {
public:
    static bool Match(EnumSizeDescription size_description, double line_length, const automationtest::utilities::Size& screen) noexcept;
    static bool Match(EnumSizeDescription size_description, const automationtest::utilities::Rectangle& rectangle, const automationtest::utilities::Size& screen) noexcept;
};

} // namespace automationtest::utilities::types

#endif // AUTOMATIOTEST_TYPES_ENUMSIZEDESCRIPTIONHELPER_HPP
