#include "DependencyStore.hpp"

#include <stdexcept>

namespace automationtest::utilities::dependency_injection {

DependencyStore& DependencyStore::Instance()
{
    static DependencyStore instance;
    return instance;
}

void DependencyStore::Initialize()
{
}

void DependencyStore::AddType(const std::string& type_name, std::any object)
{
    keyed_types_[type_name] = std::move(object);
}

std::any DependencyStore::GetType(const std::string& type_name) const
{
    const auto it = keyed_types_.find(type_name);
    if (it == keyed_types_.end()) {
        throw std::runtime_error("Cannot find type: " + type_name);
    }
    return it->second;
}

std::optional<std::any> DependencyStore::TryGetType(const std::string& type_name) const
{
    const auto it = keyed_types_.find(type_name);
    if (it == keyed_types_.end()) {
        return std::nullopt;
    }
    return it->second;
}

} // namespace automationtest::utilities::dependency_injection
