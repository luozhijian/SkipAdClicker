#ifndef AUTOMATIOTEST_REFLECTION_REGISTEREDMEMBER_HPP
#define AUTOMATIOTEST_REFLECTION_REGISTEREDMEMBER_HPP

#include "Reflection.hpp"

#include <any>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace automationtest::utilities::reflection {

struct RegisteredParameter {
    std::string name {};
    std::function<std::any(const std::string&)> converter {};
    std::any default_value {};
    bool has_default_value {false};
    bool is_out {false};
};

using RegisteredMethodInvoker = std::function<std::any(const std::vector<std::any>&)>;

struct RegisteredMethod {
    std::string name {};
    std::string declaring_type {};
    bool is_static {true};
    RegisteredMethodInvoker invoke {};
    std::vector<RegisteredParameter> parameters {};

    [[nodiscard]] std::size_t MinimumParameterCount() const noexcept;
    [[nodiscard]] std::size_t MaximumParameterCount() const noexcept;
    [[nodiscard]] bool Accepts(std::size_t parameter_count) const noexcept;
};

struct RegisteredProperty {
    std::string declaring_type {};
    std::function<std::any()> getter {};
};

struct RegisteredDll {
    std::string name {};
    std::string file_path {};
    std::string description {};
    std::string version {};
    std::string company {};
    std::vector<RegisteredMethod> methods {};
};

[[nodiscard]] std::optional<RegisteredDll> LoadRegisteredDllFromJson(const std::string& json);
[[nodiscard]] std::optional<RegisteredDll> LoadRegisteredDllFromJsonFile(const std::string& file_path);

} // namespace automationtest::utilities::reflection

AUTOMATIONTEST_REFLECT_TYPE(::automationtest::utilities::reflection::RegisteredParameter, "name", "converter", "default_value", "has_default_value", "is_out")
AUTOMATIONTEST_REFLECT_TYPE(::automationtest::utilities::reflection::RegisteredMethod, "name", "declaring_type", "is_static", "invoke", "parameters")
AUTOMATIONTEST_REFLECT_TYPE(::automationtest::utilities::reflection::RegisteredProperty, "declaring_type", "getter")
AUTOMATIONTEST_REFLECT_TYPE(::automationtest::utilities::reflection::RegisteredDll, "name", "file_path", "description", "version", "company", "methods")

#endif // AUTOMATIOTEST_REFLECTION_REGISTEREDMEMBER_HPP
