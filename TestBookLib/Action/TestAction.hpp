#ifndef AUTOMATIOTEST_TESTBOOKLIB_ACTION_TESTACTION_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_ACTION_TESTACTION_HPP

#include "../Parameter/TestActionParameter.hpp"
#include "../TestBookLibCommon.hpp"

#include <any>
#include <memory>
#include <string>
#include <vector>

namespace automationtest::testbooklib {

class TestAction {
public:
    TestAction(int line_number, std::string text);
    virtual ~TestAction() = default;

    [[nodiscard]] virtual std::string ToString() const;

    [[nodiscard]] std::any GetVariable(const std::string& string_variable) const;
    [[nodiscard]] std::any GetVariable(const std::vector<std::string>& list, int which) const;

    std::string action {};
    std::vector<parameter::TestActionParameter> parameters {};
    std::string action_text {};
    std::string return_value {};
    int line_number {0};

protected:
    [[nodiscard]] VariableService* GetVariableService() const noexcept;

    bool first_time_played {false};

private:
    mutable VariableService* variable_service_ {nullptr};
};

} // namespace automationtest::testbooklib

#endif // AUTOMATIOTEST_TESTBOOKLIB_ACTION_TESTACTION_HPP
