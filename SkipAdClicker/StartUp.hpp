#ifndef AUTOMATIOTEST_APP_STARTUP_HPP
#define AUTOMATIOTEST_APP_STARTUP_HPP

#include "../Utilities/Interface/IInteractiveAction.hpp"

#include <functional>
#include <optional>
#include <string>

class QSettings;

namespace automationtest::app {

class StartUp {
public:
    static void InitializeApplication();
    static void RegisterDefaultActionBindings();
    static void RegisterInteractiveAction(utilities::interface::IInteractiveAction* action_service);
    static void OpenOneFolder(const std::string& file_path, const std::function<bool()>& cancellation_requested = {}, const std::string& filename = {});
    static std::optional<std::string> StartupFolder();
    static QSettings AppSettings();
};

} // namespace automationtest::app

#endif // AUTOMATIOTEST_APP_STARTUP_HPP
