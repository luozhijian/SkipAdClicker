#ifndef AUTOMATIOTEST_SETTINGS_SETTINGTRIANGLEDETECTION_HPP
#define AUTOMATIOTEST_SETTINGS_SETTINGTRIANGLEDETECTION_HPP

namespace automationtest::utilities::settings {

struct SettingTriangleDetection {
    int min_area {15};
    int max_area {200};
    int min_height {3};
    int max_height {15};
    int min_width {5};
    int max_width {30};
    int min_longest_length {30};
    int max_longest_length {30};
    int in_range_lower {30};
    int in_range_upper {75};
};

} // namespace automationtest::utilities::settings

#endif // AUTOMATIOTEST_SETTINGS_SETTINGTRIANGLEDETECTION_HPP
