#ifndef AUTOMATIOTEST_COMMONTYPES_HPP
#define AUTOMATIOTEST_COMMONTYPES_HPP

#include <any>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <ostream>
#include <string>
#include <vector>

namespace automationtest::utilities {


using FuncStringToAny =
    std::function<std::any(
        const std::vector<std::string>&
    )>;


struct Point {
    int x {0};
    int y {0};

    [[nodiscard]] std::string ToString() const;
    friend bool operator==(const Point&, const Point&) = default;
};

struct Size {
    int width {0};
    int height {0};

    [[nodiscard]] bool IsEmpty() const noexcept;
    friend bool operator==(const Size&, const Size&) = default;
};

struct Rectangle {
    int x {0};
    int y {0};
    int width {0};
    int height {0};

    [[nodiscard]] int Left() const noexcept;
    [[nodiscard]] int Top() const noexcept;
    [[nodiscard]] int Right() const noexcept;
    [[nodiscard]] int Bottom() const noexcept;
    [[nodiscard]] bool IsEmpty() const noexcept;

    friend bool operator==(const Rectangle&, const Rectangle&) = default;
};

struct Color {
    std::uint8_t r {0};
    std::uint8_t g {0};
    std::uint8_t b {0};
    std::uint8_t a {255};

    friend bool operator==(const Color&, const Color&) = default;
};

struct Bitmap {
    int width {0};
    int height {0};
    int stride {0};
    std::vector<std::byte> pixels {};
    int channels {1};
};

std::ostream& operator<<(std::ostream& stream, const Point& point);
std::ostream& operator<<(std::ostream& stream, const Rectangle& rectangle);
std::ostream& operator<<(std::ostream& stream, const Color& color);

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_COMMONTYPES_HPP
