#ifndef AUTOMATIOTEST_TESTBOOKLIB_ACTION_CHECKVARIABLEEXISTS_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_ACTION_CHECKVARIABLEEXISTS_HPP

#include "BlockAction.hpp"

namespace automationtest::testbooklib::action {

class CheckVariableExists : public BlockAction {
public:
    CheckVariableExists(int line_number, const std::string& text);
    void Parse(const std::vector<std::string>& parts) override;
    void PlayActions(const ActionRunner& runner) override;

private:
    std::string variable_name_ {};
    bool check_if_exists_ {true};
    bool check_if_not_null_ {true};
    bool check_if_not_whitespace_ {true};
};

} // namespace automationtest::testbooklib::action

#endif // AUTOMATIOTEST_TESTBOOKLIB_ACTION_CHECKVARIABLEEXISTS_HPP
