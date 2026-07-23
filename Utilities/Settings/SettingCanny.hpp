#ifndef AUTOMATIOTEST_SETTINGS_SETTINGCANNY_HPP
#define AUTOMATIOTEST_SETTINGS_SETTINGCANNY_HPP
#include "SettingThreshold.hpp"

namespace automationtest::utilities::settings {

struct SettingCanny {
    int threshold1 {0};
    int threshold2 {0};
    int aperture_size {0};
    bool l2_gradient {false};
};

} // namespace automationtest::utilities::settings

#endif // AUTOMATIOTEST_SETTINGS_SETTINGCANNY_HPP
