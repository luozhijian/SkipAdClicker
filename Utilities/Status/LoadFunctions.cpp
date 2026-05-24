#include "LoadFunctions.hpp"

#include "../Settings/LoadSetting.hpp"

#include <any>
#include <cctype>

namespace automationtest::utilities::status {

namespace {

LoadFunctions::ResourceLoader MakeSettingLoaderFromFile()
{
    return [](const std::string& file_path) -> std::any {
        const auto setting = automationtest::utilities::settings::LoadSetting::LoadSettingValueFromFile(file_path);
        return setting.has_value() ? setting->second : std::any {};
    };
}

} // namespace

std::size_t CaseInsensitiveHash::operator()(const std::string& value) const noexcept
{
    std::size_t hash = 0;
    for (unsigned char ch : value) {
        hash = hash * 131 + static_cast<unsigned char>(std::tolower(ch));
    }
    return hash;
}

bool CaseInsensitiveEqual::operator()(const std::string& left, const std::string& right) const noexcept
{
    if (left.size() != right.size()) {
        return false;
    }
    for (std::size_t i = 0; i < left.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(left[i])) != std::tolower(static_cast<unsigned char>(right[i]))) {
            return false;
        }
    }
    return true;
}

LoadFunctions& LoadFunctions::Instance()
{
    static LoadFunctions instance;
    instance.Initialize();
    return instance;
}

void LoadFunctions::Initialize()
{
    if (initialized_) {
        return;
    }
    initialized_ = true;

    const auto loader = MakeSettingLoaderFromFile();
    RegisterSettingType("SettingCanny", "SettingCanny", loader);
    RegisterSettingType("SettingFindIcon", "SettingFindIcon", loader);
    RegisterSettingType("SettingFormatSet", "SettingFormatSet", loader);
    RegisterSettingType("SettingLineDetection", "SettingLineDetection", loader);
    RegisterSettingType("SettingLoadMatGray", "SettingLoadMatGray", loader);
    RegisterSettingType("SettingRectangleDetection", "SettingRectangleDetection", loader);
    RegisterSettingType("SettingSqlblock", "SettingSqlblock", loader);
    RegisterSettingType("SettingTextBlock", "SettingTextBlock", loader);
    RegisterSettingType("SettingThreshold", "SettingThreshold", loader);
    RegisterSettingType("SettingTriangleDetection", "SettingTriangleDetection", loader);
}

void LoadFunctions::RegisterMethod(const std::string& name, RegisteredMethod method)
{
    methods_[name].push_back(std::move(method));
}

void LoadFunctions::RegisterMethod(const std::string& name, RegisteredFunction function)
{
    RegisteredMethod method {};
    method.invoke = [registered = std::move(function)](const std::vector<std::any>&) -> std::any {
        registered();
        return {};
    };
    RegisterMethod(name, std::move(method));
}

bool LoadFunctions::LoadRegistedDll(const std::string& file_path)
{
    auto registered_dll = automationtest::utilities::reflection::LoadRegisteredDllFromJsonFile(file_path);
    if (!registered_dll.has_value()) {
        return false;
    }

    for (const auto& method : registered_dll->methods) {
        const auto method_name = method.name;
        if (!method_name.empty()) {
            RegisterMethod(method_name, method);
        }
    }
    registered_dlls_.push_back(std::move(registered_dll.value()));
    return true;
}

bool LoadFunctions::LoadRegisteredDll(const std::string& file_path)
{
    return LoadRegistedDll(file_path);
}

std::optional<LoadFunctions::RegisteredMethod> LoadFunctions::GetMethod(const std::string& name, std::size_t parameter_count) const
{
    const auto it = methods_.find(name);
    if (it == methods_.end()) {
        return std::nullopt;
    }

    for (const auto& method : it->second) {
        if (method.parameters.size() == parameter_count) {
            return method;
        }
    }
    for (const auto& method : it->second) {
        if (method.Accepts(parameter_count)) {
            return method;
        }
    }
    return std::nullopt;
}

std::optional<LoadFunctions::RegisteredMethod> LoadFunctions::GetMethod(const std::string& name) const
{
    const auto it = methods_.find(name);
    if (it == methods_.end() || it->second.empty()) {
        return std::nullopt;
    }
    return it->second.front();
}

void LoadFunctions::RegisterProperty(const std::string& name, RegisteredProperty property)
{
    properties_[name] = std::move(property);
}

std::optional<LoadFunctions::RegisteredProperty> LoadFunctions::GetProperty(const std::string& name) const
{
    const auto it = properties_.find(name);
    if (it == properties_.end()) {
        return std::nullopt;
    }
    return it->second;
}

void LoadFunctions::RegisterSelfDefinedFunction(const std::string& name, automationtest::testbooklib::flowcontrols::DefFunction* function)
{
    self_defined_functions_[name] = function;
}

automationtest::testbooklib::flowcontrols::DefFunction* LoadFunctions::GetSelfDefinedFunction(const std::string& name) const
{
    const auto it = self_defined_functions_.find(name);
    if (it == self_defined_functions_.end()) {
        return nullptr;
    }
    return it->second;
}

void LoadFunctions::RegisterSettingType(const std::string& name, const std::string& type_name, ResourceLoader loader)
{
    setting_types_[name] = type_name;
    if (loader) {
        setting_loaders_[name] = std::move(loader);
    }
}

std::optional<std::string> LoadFunctions::GetSettingType(const std::string& name) const
{
    const auto it = setting_types_.find(name);
    if (it == setting_types_.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::optional<LoadFunctions::ResourceLoader> LoadFunctions::GetSettingLoader(const std::string& name) const
{
    const auto it = setting_loaders_.find(name);
    if (it == setting_loaders_.end()) {
        return std::nullopt;
    }
    return it->second;
}

const std::vector<LoadFunctions::RegisteredDll>& LoadFunctions::GetRegisteredDlls() const noexcept
{
    return registered_dlls_;
}

} // namespace automationtest::utilities::status
