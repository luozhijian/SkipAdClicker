#ifndef AUTOMATIOTEST_SETTINGS_SETTINGLOADMATGRAY_HPP
#define AUTOMATIOTEST_SETTINGS_SETTINGLOADMATGRAY_HPP

#include <string>

namespace automationtest::utilities::settings {

struct SettingLoadMatGray {
    std::string apply_threshold {};
    int threshold_value {0};
    int threshold_max_value {0};
};

} // namespace automationtest::utilities::settings

#endif // AUTOMATIOTEST_SETTINGS_SETTINGLOADMATGRAY_HPP
