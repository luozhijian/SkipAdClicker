#ifndef AUTOMATIOTEST_STATUS_LAYOUTDESCRIPTION_HPP
#define AUTOMATIOTEST_STATUS_LAYOUTDESCRIPTION_HPP

#include <optional>
#include <string>

namespace automationtest::utilities::status {

struct LayoutDescription {
    std::string name {};
    std::optional<int> x {};
    std::optional<int> y {};
    std::optional<int> width {};
    std::optional<int> height {};
    std::optional<int> end_x {};
    std::optional<int> end_y {};
};

} // namespace automationtest::utilities::status

#endif // AUTOMATIOTEST_STATUS_LAYOUTDESCRIPTION_HPP
