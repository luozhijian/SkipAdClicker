#ifndef AUTOMATIOTEST_USERINTERFACELIB_CONTROLS_UIDESCRIPTION_HPP
#define AUTOMATIOTEST_USERINTERFACELIB_CONTROLS_UIDESCRIPTION_HPP

#include <string>
#include <vector>

namespace automationtest::userinterfacelib {

class UiDescription {
public:
    std::string caption {};
    std::vector<std::string> items {};
};

} // namespace automationtest::userinterfacelib

#endif // AUTOMATIOTEST_USERINTERFACELIB_CONTROLS_UIDESCRIPTION_HPP
