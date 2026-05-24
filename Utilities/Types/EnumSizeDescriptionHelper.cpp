#include "EnumSizeDescriptionHelper.hpp"

#include <algorithm>
#include <cmath>

namespace automationtest::utilities::types {

bool EnumSizeDescriptionHelper::Match(
    EnumSizeDescription size_description,
    double line_length,
    const automationtest::utilities::Size& screen) noexcept
{
    if (screen.IsEmpty()) {
        return false;
    }

    if (size_description == EnumSizeDescription::Full) {
        return std::abs(line_length - static_cast<double>(screen.width))
            / std::max(1, screen.width) < 0.05;
    }

    return false;
}

bool EnumSizeDescriptionHelper::Match(
    EnumSizeDescription size_description,
    const automationtest::utilities::Rectangle& rectangle,
    const automationtest::utilities::Size& screen) noexcept
{
    if (screen.IsEmpty()) {
        return false;
    }

    if (size_description == EnumSizeDescription::Full) {
        return std::abs(rectangle.width - screen.width) / std::max(1, screen.width) < 0.01
            && std::abs(rectangle.height - screen.height) / std::max(1, screen.height) < 0.01;
    }

    return false;
}

} // namespace automationtest::utilities::types
