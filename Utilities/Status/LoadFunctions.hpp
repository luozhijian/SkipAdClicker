#ifndef AUTOMATIOTEST_STATUS_LOADFUNCTIONS_HPP
#define AUTOMATIOTEST_STATUS_LOADFUNCTIONS_HPP

#include "../Reflection/RegisteredMember.hpp"

#include <any>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace automationtest::testbooklib::flowcontrols {
class DefFunction;
}
    
namespace automationtest::utilities::status {

struct CaseInsensitiveHash {
    std::size_t operator()(const std::string& value) const noexcept;
};

struct CaseInsensitiveEqual {
    bool operator()(const std::string& left, const std::string& right) const noexcept;
};

class LoadFunctions {
public:
    using RegisteredFunction = std::function<void()>;
    using ResourceLoader = std::function<std::any(const std::string&)>;
    using RegisteredParameter = automationtest::utilities::reflection::RegisteredParameter;
    using RegisteredMethodInvoker = automationtest::utilities::reflection::RegisteredMethodInvoker;
    using RegisteredMethod = automationtest::utilities::reflection::RegisteredMethod;
    using RegisteredProperty = automationtest::utilities::reflection::RegisteredProperty;
    using RegisteredDll = automationtest::utilities::reflection::RegisteredDll;

    static LoadFunctions& Instance();

    void Initialize();
    void RegisterMethod(const std::string& name, RegisteredMethod method);
    void RegisterMethod(const std::string& name, RegisteredFunction function);
    bool LoadRegistedDll(const std::string& file_path);
    bool LoadRegisteredDll(const std::string& file_path);
    [[nodiscard]] std::optional<RegisteredMethod> GetMethod(const std::string& name, std::size_t parameter_count) const;
    [[nodiscard]] std::optional<RegisteredMethod> GetMethod(const std::string& name) const;
    void RegisterProperty(const std::string& name, RegisteredProperty property);
    [[nodiscard]] std::optional<RegisteredProperty> GetProperty(const std::string& name) const;
    void RegisterSelfDefinedFunction(const std::string& name, automationtest::testbooklib::flowcontrols::DefFunction* function);
    [[nodiscard]] automationtest::testbooklib::flowcontrols::DefFunction* GetSelfDefinedFunction(const std::string& name) const;
    void RegisterSettingType(const std::string& name, const std::string& type_name, ResourceLoader loader = {});
    [[nodiscard]] std::optional<std::string> GetSettingType(const std::string& name) const;
    [[nodiscard]] std::optional<ResourceLoader> GetSettingLoader(const std::string& name) const;
    [[nodiscard]] const std::vector<RegisteredDll>& GetRegisteredDlls() const noexcept;

private:
    LoadFunctions() = default;

    std::unordered_map<std::string, std::vector<RegisteredMethod>, CaseInsensitiveHash, CaseInsensitiveEqual> methods_ {};
    std::unordered_map<std::string, RegisteredProperty, CaseInsensitiveHash, CaseInsensitiveEqual> properties_ {};
    std::unordered_map<std::string, automationtest::testbooklib::flowcontrols::DefFunction*, CaseInsensitiveHash, CaseInsensitiveEqual> self_defined_functions_ {};
    std::unordered_map<std::string, std::string, CaseInsensitiveHash, CaseInsensitiveEqual> setting_types_ {};
    std::unordered_map<std::string, ResourceLoader, CaseInsensitiveHash, CaseInsensitiveEqual> setting_loaders_ {};
    std::vector<RegisteredDll> registered_dlls_ {};
    bool initialized_ {false};
};

} // namespace automationtest::utilities::status

#endif // AUTOMATIOTEST_STATUS_LOADFUNCTIONS_HPP
