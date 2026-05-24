#ifndef AUTOMATIOTEST_STATUS_WINDOWLOCATIONSETTING_HPP
#define AUTOMATIOTEST_STATUS_WINDOWLOCATIONSETTING_HPP

#include <optional>
#include <string>

namespace automationtest::utilities::status {

struct WindowLocationSetting {
    std::optional<int> location_x {};
    std::optional<int> location_y {};
    std::string form_border_style {};
};

} // namespace automationtest::utilities::status

#endif // AUTOMATIOTEST_STATUS_WINDOWLOCATIONSETTING_HPP
