#ifndef AUTOMATIOTEST_GLOBALVARIABLES_HPP
#define AUTOMATIOTEST_GLOBALVARIABLES_HPP

#include "Interface/IRecentActionService.hpp"

#include <memory>

namespace automationtest::utilities {

class GlobalVariables {
public:
    static std::shared_ptr<interface::IRecentActionService> recent_action_service;
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_GLOBALVARIABLES_HPP
