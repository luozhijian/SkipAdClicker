#ifndef AUTOMATIOTEST_TESTBOOKLIB_ACTION_RETRYWHENEXCEPTION_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_ACTION_RETRYWHENEXCEPTION_HPP

#include "BlockAction.hpp"

namespace automationtest::testbooklib::action {

class RetryWhenException : public BlockAction {
public:
    RetryWhenException(int line_number, const std::string& text);
    void Parse(const std::vector<std::string>& parts) override;
    void PlayActions(const ActionRunner& runner) override;

private:
    int retry_count_ {0};
};

} // namespace automationtest::testbooklib::action

#endif // AUTOMATIOTEST_TESTBOOKLIB_ACTION_RETRYWHENEXCEPTION_HPP
