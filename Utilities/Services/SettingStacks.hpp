#ifndef AUTOMATIOTEST_SERVICES_SETTINGSTACKS_HPP
#define AUTOMATIOTEST_SERVICES_SETTINGSTACKS_HPP

#include <any>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace automationtest::utilities::services {

class SettingStacks {
public:
    bool AddStackedType(const std::string& type_name);
    [[nodiscard]] std::optional<std::any> GetStackedType(const std::string& type_name) const;
    bool AddObject(const std::string& type_name, std::any value);

private:
    std::unordered_map<std::string, std::vector<std::any>> stacked_types_ {};
};

} // namespace automationtest::utilities::services

#endif // AUTOMATIOTEST_SERVICES_SETTINGSTACKS_HPP
