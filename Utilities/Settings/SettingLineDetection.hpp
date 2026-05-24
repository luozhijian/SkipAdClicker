#ifndef AUTOMATIOTEST_SETTINGS_SETTINGLINEDETECTION_HPP
#define AUTOMATIOTEST_SETTINGS_SETTINGLINEDETECTION_HPP

#include "SettingCanny.hpp"
#include "SettingThreshold.hpp"

namespace automationtest::utilities::settings {

struct SettingLineDetection {
    SettingCanny setting_canny {};
    SettingThreshold setting_threshold {};
    double angle_resolution_in_radians {3.14159265358979323846 / 180.0};
    int hough_lines_threshold {2};
    int hough_lines_min_line_length {5};
    int hough_lines_max_line_gap {0};
};

} // namespace automationtest::utilities::settings

#endif // AUTOMATIOTEST_SETTINGS_SETTINGLINEDETECTION_HPP
