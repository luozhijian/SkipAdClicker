#ifndef AUTOMATIOTEST_TYPES_SHORTLINE_HPP
#define AUTOMATIOTEST_TYPES_SHORTLINE_HPP

#include "ShortPoint.hpp"

#include <cstdint>
#include <tuple>

namespace automationtest::utilities::types {

union ShortLine {
    std::int64_t long_value;
    struct {
        std::int16_t x1;
        std::int16_t y1;
        std::int16_t x2;
        std::int16_t y2;
    };
    struct {
        std::int32_t p1;
        std::int32_t p2;
    };

    ShortLine() noexcept;
    ShortLine(ShortPoint start, ShortPoint end) noexcept;

    [[nodiscard]] ShortPoint Point1() const noexcept;
    [[nodiscard]] ShortPoint Point2() const noexcept;
    [[nodiscard]] std::tuple<std::int16_t, std::int16_t, std::int16_t, std::int16_t> ToShortXY() const noexcept;
    [[nodiscard]] std::pair<ShortPoint, ShortPoint> ToShortPoints() const noexcept;
    [[nodiscard]] std::int16_t MinX() const noexcept;
    [[nodiscard]] std::int16_t MaxX() const noexcept;
    [[nodiscard]] std::int16_t MinY() const noexcept;
    [[nodiscard]] std::int16_t MaxY() const noexcept;
};

static_assert(sizeof(ShortLine) == sizeof(std::int64_t));

} // namespace automationtest::utilities::types

#endif // AUTOMATIOTEST_TYPES_SHORTLINE_HPP
