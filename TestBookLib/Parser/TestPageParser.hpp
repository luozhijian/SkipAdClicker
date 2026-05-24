#ifndef AUTOMATIOTEST_TESTBOOKLIB_PARSER_TESTPAGEPARSER_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_PARSER_TESTPAGEPARSER_HPP

#include "../Action/BlockAction.hpp"
#include "../TestPage.hpp"
#include "TestActionParser.hpp"

#include <memory>
#include <string>
#include <vector>

namespace automationtest::testbooklib::parser {

class TestPageParser {
public:
    std::unique_ptr<TestPage> Parse(const std::string& file_path);

private:
    struct ParsedNode {
        int line_number {0};
        std::string text {};
        std::vector<ParsedNode> children {};
    };

    static std::vector<ParsedNode> ParseFileToTree(const std::string& file_path);
    static std::string PrintTree(const std::vector<ParsedNode>& nodes, int indent_level = 0);
    void ParseNodes(const std::vector<ParsedNode>& nodes, action::BlockAction& block_action, TestActionParser& action_parser);
};

} // namespace automationtest::testbooklib::parser

#endif // AUTOMATIOTEST_TESTBOOKLIB_PARSER_TESTPAGEPARSER_HPP
