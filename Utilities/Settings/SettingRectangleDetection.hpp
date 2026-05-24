#ifndef AUTOMATIOTEST_SETTINGS_SETTINGRECTANGLEDETECTION_HPP
#define AUTOMATIOTEST_SETTINGS_SETTINGRECTANGLEDETECTION_HPP

namespace automationtest::utilities::settings {

struct SettingRectangleDetection {
    int min_area {200};
    int max_area {2147483647};
    int min_height {10};
    int max_height {2147483647};
    int min_width {50};
    int max_width {2147483647};
    int canny_threshold {100};
    int canny_threshold_linking {200};
    int line_min_length {20};
    int point_merge_delta {2};
    double epsilon {5.0};
};

} // namespace automationtest::utilities::settings

#endif // AUTOMATIOTEST_SETTINGS_SETTINGRECTANGLEDETECTION_HPP
