#ifndef AUTOMATIOTEST_INTERFACE_IVARIABLESERVICE_HPP
#define AUTOMATIOTEST_INTERFACE_IVARIABLESERVICE_HPP

#include <any>
#include <string>

namespace automationtest::utilities::interface {

class IVariableService {
public:
    virtual ~IVariableService() = default;

    virtual bool TryGetValue(const std::string& name, std::any& value) const = 0;
    virtual void SetObject(const std::string& name, std::any value) = 0;
    [[nodiscard]] virtual std::any SolveVariable(const std::any& name) const = 0;
    [[nodiscard]] virtual std::any SolveResource(const std::string& name) const = 0;
    [[nodiscard]] virtual bool ContainVariable(const std::string& name) const = 0;
    virtual bool RemoveVariable(const std::string& name) = 0;
};

} // namespace automationtest::utilities::interface

#endif // AUTOMATIOTEST_INTERFACE_IVARIABLESERVICE_HPP
