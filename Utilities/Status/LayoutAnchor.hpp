#ifndef AUTOMATIOTEST_STATUS_LAYOUTANCHOR_HPP
#define AUTOMATIOTEST_STATUS_LAYOUTANCHOR_HPP

#include <string>

namespace automationtest::utilities::status {

struct LayoutAnchor {
    std::string name {};
    std::string regex {};
    std::string location {};
};

} // namespace automationtest::utilities::status

#endif // AUTOMATIOTEST_STATUS_LAYOUTANCHOR_HPP
