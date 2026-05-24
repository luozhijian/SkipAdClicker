#ifndef AUTOMATIOTEST_PLAYTESTBOOK_SERVICES_TESTPARAMETERHELPER_HPP
#define AUTOMATIOTEST_PLAYTESTBOOK_SERVICES_TESTPARAMETERHELPER_HPP

#include "../../TestBookLib/FlowControls/DefFunction.hpp"

#include <any>
#include <string>

namespace automationtest::playtestbook::services {

class TestParameterHelper {
public:
    static bool ProcessSpecialParameter(const std::string& parameter, std::any& object, testbooklib::flowcontrols::DefFunction* current_function);

private:
    static std::any GenerateSpecialParameter(const std::string& special, testbooklib::flowcontrols::DefFunction* current_function);
};

} // namespace automationtest::playtestbook::services

#endif // AUTOMATIOTEST_PLAYTESTBOOK_SERVICES_TESTPARAMETERHELPER_HPP
