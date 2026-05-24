#ifndef AUTOMATIOTEST_STATUS_GRIDSETTING_HPP
#define AUTOMATIOTEST_STATUS_GRIDSETTING_HPP

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace automationtest::utilities::status {

struct RgbColor {
    std::uint8_t r {0};
    std::uint8_t g {0};
    std::uint8_t b {0};
};

struct GridSetting {
    std::string name {};
    std::string description {};
    std::optional<int> row_count {};
    std::vector<RgbColor> broad_colors {};
    std::vector<RgbColor> grid_line_colors {};
};

} // namespace automationtest::utilities::status

#endif // AUTOMATIOTEST_STATUS_GRIDSETTING_HPP
