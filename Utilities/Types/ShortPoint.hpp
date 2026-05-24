#ifndef AUTOMATIOTEST_TYPES_SHORTPOINT_HPP
#define AUTOMATIOTEST_TYPES_SHORTPOINT_HPP

#include "../CommonTypes.hpp"

#include <cstdint>

namespace automationtest::utilities::types {

union ShortPoint {
    struct {
        std::int16_t x;
        std::int16_t y;
    };
    std::int32_t packed_value;

    ShortPoint() noexcept;
    ShortPoint(int x_value, int y_value) noexcept;

    [[nodiscard]] automationtest::utilities::Point ToPoint() const noexcept;
};

static_assert(sizeof(ShortPoint) == sizeof(std::int32_t));

} // namespace automationtest::utilities::types

#endif // AUTOMATIOTEST_TYPES_SHORTPOINT_HPP
