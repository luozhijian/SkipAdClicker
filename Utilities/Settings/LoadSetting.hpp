#ifndef AUTOMATIOTEST_SETTINGS_LOADSETTING_HPP
#define AUTOMATIOTEST_SETTINGS_LOADSETTING_HPP

#include <any>
#include <optional>
#include <string>
#include <utility>

namespace automationtest::utilities::settings {

class LoadSetting {
public:
    static std::optional<std::pair<std::string, std::any>> LoadSettingValueFromFile(const std::string& filename);
    static void LoadingSettingFromFile(const std::string& filename);
};

} // namespace automationtest::utilities::settings

#endif // AUTOMATIOTEST_SETTINGS_LOADSETTING_HPP
