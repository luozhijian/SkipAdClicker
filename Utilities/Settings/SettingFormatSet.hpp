#ifndef AUTOMATIOTEST_SETTINGS_SETTINGFORMATSET_HPP
#define AUTOMATIOTEST_SETTINGS_SETTINGFORMATSET_HPP

#include <string>

namespace automationtest::utilities::settings {

struct SettingFormatSet {
    std::string setting_find_icon {};
    std::string setting_line_detection {};
    std::string setting_rectangle_detection {};
    std::string setting_text_block {};
    std::string setting_triangle_detection {};
};

} // namespace automationtest::utilities::settings

#endif // AUTOMATIOTEST_SETTINGS_SETTINGFORMATSET_HPP
