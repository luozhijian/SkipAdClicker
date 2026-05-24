#ifndef AUTOMATIOTEST_TESTBOOKLIB_TESTPAGE_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_TESTPAGE_HPP

#include "Action/BlockAction.hpp"

namespace automationtest::testbooklib {

class TestPage : public action::BlockAction {
public:
    TestPage(int line_number, const std::string& text);
    void Parse(const std::vector<std::string>& parts) override;
};

} // namespace automationtest::testbooklib

#endif // AUTOMATIOTEST_TESTBOOKLIB_TESTPAGE_HPP
