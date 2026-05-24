#ifndef AUTOMATIOTEST_SETTINGS_SETTINGTEXTBLOCK_HPP
#define AUTOMATIOTEST_SETTINGS_SETTINGTEXTBLOCK_HPP

#include <string>

namespace automationtest::utilities::settings {

struct SettingTextBlock {
    bool perform_pyr_down {true};
    int morphology_x {3};
    int morphology_y {3};
    bool perform_second_morphology {true};
    int morphology2_x {3};
    int morphology2_y {3};
    std::string ocr_language {};
    int canny_threshold1 {200};
    int canny_threshold2 {200};
};

} // namespace automationtest::utilities::settings

#endif // AUTOMATIOTEST_SETTINGS_SETTINGTEXTBLOCK_HPP
