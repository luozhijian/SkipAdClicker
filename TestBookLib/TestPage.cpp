#include "TestPage.hpp"

namespace automationtest::testbooklib {

TestPage::TestPage(int line_number, const std::string& text)
    : action::BlockAction(line_number, text)
{
}

void TestPage::Parse(const std::vector<std::string>& parts)
{
    (void)parts;
}

} // namespace automationtest::testbooklib
