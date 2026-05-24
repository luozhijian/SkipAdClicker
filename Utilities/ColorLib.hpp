#ifndef AUTOMATIOTEST_COLORLIB_HPP
#define AUTOMATIOTEST_COLORLIB_HPP

#include "CommonTypes.hpp"

#include <string>

namespace automationtest::utilities {

class ColorLib {
public:
    static bool TwoColorIsSame(const Color& c1, const Color& c2) noexcept;
    static bool TwoColorIsSimiliar(const Color& c1, const Color& c2) noexcept;
    static std::uint8_t ColorToGray256(const Color& color) noexcept;
    static std::string ToHex(const Color& color);
    static std::string ToRGB(const Color& color);
    static Color FromHexColor(const std::string& color);
    static Color FromRGBColor(const std::string& color);
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_COLORLIB_HPP
