#ifndef AUTOMATIOTEST_APP_SERVICES_CHROMESERVICE_HPP
#define AUTOMATIOTEST_APP_SERVICES_CHROMESERVICE_HPP

#include "ApplicationService.hpp"

namespace automationtest::app::services {

class ChromeService : public ApplicationService {
public:
    static ChromeService& Instance();
    static void RegisterBindings(utilities::status::LoadFunctions& load_functions);
};

} // namespace automationtest::app::services

#endif // AUTOMATIOTEST_APP_SERVICES_CHROMESERVICE_HPP
