#ifndef AUTOMATIOTEST_SERVICES_VARIABLESERVICE_HPP
#define AUTOMATIOTEST_SERVICES_VARIABLESERVICE_HPP

#include "../Interface/IVariableService.hpp"
#include "SettingStacks.hpp"

#include <any>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace automationtest::utilities::services {

class VariableService : public interface::IVariableService {
public:
    static VariableService& Instance();

    bool TryGetValue(const std::string& name, std::any& value) const override;
    void SetObject(const std::string& name, std::any value) override;
    [[nodiscard]] std::any SolveVariable(const std::any& name) const override;
    [[nodiscard]] std::any SolveResource(const std::string& name) const override;
    [[nodiscard]] bool ContainVariable(const std::string& name) const override;
    bool RemoveVariable(const std::string& name) override;

    [[nodiscard]] std::optional<std::any> TryGetSettingType(const std::string& type_name) const;
    void SaveSetting(const std::string& type_name, std::any value);

private:
    VariableService() = default;

    static std::string NormalizeName(const std::string& name);

    SettingStacks setting_stacks_ {};
    std::unordered_map<std::string, std::any> resources_ {};
};

} // namespace automationtest::utilities::services

#endif // AUTOMATIOTEST_SERVICES_VARIABLESERVICE_HPP
