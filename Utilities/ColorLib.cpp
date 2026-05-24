#include "ColorLib.hpp"

#include "TypeLib.hpp"

#include <iomanip>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace automationtest::utilities {

bool ColorLib::TwoColorIsSame(const Color& c1, const Color& c2) noexcept
{
    return c1 == c2;
}

bool ColorLib::TwoColorIsSimiliar(const Color& c1, const Color& c2) noexcept
{
    return c1 == c2;
}

std::uint8_t ColorLib::ColorToGray256(const Color& color) noexcept
{
    const double value = 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;
    return value >= 255.5 ? 255 : static_cast<std::uint8_t>(value);
}

std::string ColorLib::ToHex(const Color& color)
{
    std::ostringstream stream;
    stream << "#"
           << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(color.r)
           << std::setw(2) << static_cast<int>(color.g)
           << std::setw(2) << static_cast<int>(color.b);
    return stream.str();
}

std::string ColorLib::ToRGB(const Color& color)
{
    std::ostringstream stream;
    stream << "RGB(" << static_cast<int>(color.r) << "," << static_cast<int>(color.g) << "," << static_cast<int>(color.b) << ")";
    return stream.str();
}

Color ColorLib::FromHexColor(const std::string& color)
{
    if (color.size() != 7 || color[0] != '#') {
        throw std::runtime_error("Unknown Color: " + color);
    }
    return Color {
        static_cast<std::uint8_t>(std::stoi(color.substr(1, 2), nullptr, 16)),
        static_cast<std::uint8_t>(std::stoi(color.substr(3, 2), nullptr, 16)),
        static_cast<std::uint8_t>(std::stoi(color.substr(5, 2), nullptr, 16)),
        255
    };
}

Color ColorLib::FromRGBColor(const std::string& color)
{
    static const std::regex pattern(R"(RGB\s*\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\))", std::regex_constants::icase);
    std::smatch match;
    if (!std::regex_match(color, match, pattern)) {
        throw std::runtime_error("Unknown Color: " + color);
    }

    const auto red = TypeLib::ToNullInt(match[1].str());
    const auto green = TypeLib::ToNullInt(match[2].str());
    const auto blue = TypeLib::ToNullInt(match[3].str());
    if (!red.has_value() || !green.has_value() || !blue.has_value()) {
        throw std::runtime_error("Unknown Color: " + color);
    }

    return Color {
        static_cast<std::uint8_t>(*red),
        static_cast<std::uint8_t>(*green),
        static_cast<std::uint8_t>(*blue),
        255
    };
}

} // namespace automationtest::utilities
