#ifndef AUTOMATIOTEST_TESTBOOKLIB_ACTION_CHECKVARIABLEISEMPTYORWHITESPACE_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_ACTION_CHECKVARIABLEISEMPTYORWHITESPACE_HPP

#include "BlockAction.hpp"

namespace automationtest::testbooklib::action {

class CheckVariableIsEmptyOrWhiteSpace : public BlockAction {
public:
    CheckVariableIsEmptyOrWhiteSpace(int line_number, const std::string& text);
    void Parse(const std::vector<std::string>& parts) override;
    void PlayActions(const ActionRunner& runner) override;

private:
    std::string variable_name_ {};
};

} // namespace automationtest::testbooklib::action

#endif // AUTOMATIOTEST_TESTBOOKLIB_ACTION_CHECKVARIABLEISEMPTYORWHITESPACE_HPP
