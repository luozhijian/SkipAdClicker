#include "ColorJsonConverter.hpp"

#include "ColorLib.hpp"

namespace automationtest::utilities {

Color ColorJsonConverter::Read(const std::string& color_hex)
{
    return ColorLib::FromHexColor(color_hex);
}

std::string ColorJsonConverter::Write(const Color& color)
{
    return ColorLib::ToHex(color);
}

} // namespace automationtest::utilities
