#ifndef AUTOMATIOTEST_COLORJSONCONVERTER_HPP
#define AUTOMATIOTEST_COLORJSONCONVERTER_HPP

#include "CommonTypes.hpp"

#include <string>

namespace automationtest::utilities {

class ColorJsonConverter {
public:
    static Color Read(const std::string& color_hex);
    static std::string Write(const Color& color);
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_COLORJSONCONVERTER_HPP
