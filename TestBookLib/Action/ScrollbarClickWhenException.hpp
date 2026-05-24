#ifndef AUTOMATIOTEST_TESTBOOKLIB_ACTION_SCROLLBARCLICKWHENEXCEPTION_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_ACTION_SCROLLBARCLICKWHENEXCEPTION_HPP

#include "BlockAction.hpp"

namespace automationtest::testbooklib::action {

class ScrollbarClickWhenException : public BlockAction {
public:
    ScrollbarClickWhenException(int line_number, const std::string& text);
    void Parse(const std::vector<std::string>& parts) override;
    void PlayActions(const ActionRunner& runner) override;
};

} // namespace automationtest::testbooklib::action

#endif // AUTOMATIOTEST_TESTBOOKLIB_ACTION_SCROLLBARCLICKWHENEXCEPTION_HPP
