#ifndef AUTOMATIOTEST_TESTBOOKLIB_PARSER_TESTACTIONPARSER_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_PARSER_TESTACTIONPARSER_HPP

#include "../Action/ActionFactory.hpp"

namespace automationtest::testbooklib::parser {

class TestActionParser {
public:
    std::unique_ptr<TestAction> Parse(int line_number, const std::string& action_line);

private:
    std::unique_ptr<TestAction> ProcessLetIfItIsLet(int line_number, const std::string& action_line);
};

} // namespace automationtest::testbooklib::parser

#endif // AUTOMATIOTEST_TESTBOOKLIB_PARSER_TESTACTIONPARSER_HPP
