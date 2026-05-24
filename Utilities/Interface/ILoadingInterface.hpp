#ifndef AUTOMATIOTEST_INTERFACE_ILOADINGINTERFACE_HPP
#define AUTOMATIOTEST_INTERFACE_ILOADINGINTERFACE_HPP

#include "IVariableService.hpp"

#include <string>

namespace automationtest::utilities::interface {

class ILoadingInterface {
public:
    virtual ~ILoadingInterface() = default;
    virtual void AddTo(IVariableService& service, const std::string& name) = 0;
};

} // namespace automationtest::utilities::interface

#endif // AUTOMATIOTEST_INTERFACE_ILOADINGINTERFACE_HPP
