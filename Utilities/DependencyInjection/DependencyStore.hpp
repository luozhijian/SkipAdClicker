#ifndef AUTOMATIOTEST_DEPENDENCYINJECTION_DEPENDENCYSTORE_HPP
#define AUTOMATIOTEST_DEPENDENCYINJECTION_DEPENDENCYSTORE_HPP

#include "../Interface/IInteractiveAction.hpp"

#include <any>
#include <optional>
#include <string>
#include <unordered_map>

namespace automationtest::utilities::dependency_injection {

class DependencyStore {
public:
    static DependencyStore& Instance();

    interface::IInteractiveAction* interactive_action {nullptr};

    void Initialize();
    void AddType(const std::string& type_name, std::any object);
    [[nodiscard]] std::any GetType(const std::string& type_name) const;
    [[nodiscard]] std::optional<std::any> TryGetType(const std::string& type_name) const;

private:
    DependencyStore() = default;

    std::unordered_map<std::string, std::any> keyed_types_ {};
};

} // namespace automationtest::utilities::dependency_injection

#endif // AUTOMATIOTEST_DEPENDENCYINJECTION_DEPENDENCYSTORE_HPP
