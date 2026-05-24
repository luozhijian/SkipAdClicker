#ifndef AUTOMATIOTEST_APP_SERVICES_APPLICATIONSERVICEBASE_HPP
#define AUTOMATIOTEST_APP_SERVICES_APPLICATIONSERVICEBASE_HPP

namespace automationtest::app::services {

class ApplicationServiceBase {
public:
    static constexpr int VK_RETURN = 0x0D;
    static constexpr int VK_TAB = 0x09;
    static constexpr int VK_RIGHT = 0x27;
    static constexpr int VK_LEFT = 0x25;
    static constexpr int VK_UP = 0x26;
    static constexpr int VK_DOWN = 0x28;
    static constexpr int VK_PRIOR = 0x21;
    static constexpr int VK_NEXT = 0x22;
};

} // namespace automationtest::app::services

#endif // AUTOMATIOTEST_APP_SERVICES_APPLICATIONSERVICEBASE_HPP
