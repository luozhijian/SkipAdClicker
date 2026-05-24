#include "eOrientation.hpp"

namespace automationtest::utilities {

eOrientation eOrientationHelper::Rotate(eOrientation orientation) noexcept
{
    return orientation == eOrientation::Horizontal
        ? eOrientation::Vertical
        : eOrientation::Horizontal;
}

} // namespace automationtest::utilities
