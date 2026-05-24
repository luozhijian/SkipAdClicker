#ifndef AUTOMATIOTEST_PLAYTESTBOOK_SERVICES_STATICRESOURCESERVICE_HPP
#define AUTOMATIOTEST_PLAYTESTBOOK_SERVICES_STATICRESOURCESERVICE_HPP

#include "../../Utilities/Services/VariableService.hpp"

namespace automationtest::playtestbook::services {

class StaticResourceService {
public:
    static StaticResourceService& Instance();

    int LoadObjects(const std::string& folder, utilities::services::VariableService& variable_service);

private:
    StaticResourceService() = default;
};

} // namespace automationtest::playtestbook::services

#endif // AUTOMATIOTEST_PLAYTESTBOOK_SERVICES_STATICRESOURCESERVICE_HPP
