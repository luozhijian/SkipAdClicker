#ifndef AUTOMATIOTEST_APP_APPLICATIONAUTOSTART_HPP
#define AUTOMATIOTEST_APP_APPLICATIONAUTOSTART_HPP

class QString;

namespace automationtest::app {

class ApplicationAutoStart {
public:
    static bool IsEnabled();
    static bool UsesCurrentApplication();
    static bool SetEnabled(bool enabled, QString* error_message = nullptr);
};

} // namespace automationtest::app

#endif // AUTOMATIOTEST_APP_APPLICATIONAUTOSTART_HPP
