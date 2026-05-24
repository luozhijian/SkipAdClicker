#ifndef AUTOMATIOTEST_SETTINGS_SETTINGFINDICON_HPP
#define AUTOMATIOTEST_SETTINGS_SETTINGFINDICON_HPP

#include "SettingCanny.hpp"

#include <string>

namespace automationtest::utilities::settings {

struct SettingFindIcon {
    std::string apply_threshold {};
    int threshold_value {0};
    int threshold_max_value {0};
    std::string scales {"1"};
    double threshold {0.5};
    SettingCanny setting_canny {};
};

} // namespace automationtest::utilities::settings

#endif // AUTOMATIOTEST_SETTINGS_SETTINGFINDICON_HPP
