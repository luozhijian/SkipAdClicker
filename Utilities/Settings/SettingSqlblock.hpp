#ifndef AUTOMATIOTEST_SETTINGS_SETTINGSQLBLOCK_HPP
#define AUTOMATIOTEST_SETTINGS_SETTINGSQLBLOCK_HPP

#include <string>
#include <unordered_map>
#include <vector>

namespace automationtest::utilities::settings {

struct OneSqlDefinition {
    std::string name {};
    std::string connection_string {};
    std::string sql {};
};

struct SettingSqlblock {
    std::unordered_map<std::string, std::string> connection_strings {};
    std::vector<OneSqlDefinition> sqls {};

    void ResolveConnectionStrings();
};

} // namespace automationtest::utilities::settings

#endif // AUTOMATIOTEST_SETTINGS_SETTINGSQLBLOCK_HPP
