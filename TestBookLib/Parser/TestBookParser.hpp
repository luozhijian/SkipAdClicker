#ifndef AUTOMATIOTEST_TESTBOOKLIB_PARSER_TESTBOOKPARSER_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_PARSER_TESTBOOKPARSER_HPP

#include "../TestBook.hpp"

#include <string>

namespace automationtest::testbooklib::parser {

class TestBookParser {
public:
    static constexpr const char* StaticResourceFileName = "StaticResources.txt";

    TestBook Parse(const std::string& path, const std::string& filename = {});
};

} // namespace automationtest::testbooklib::parser

#endif // AUTOMATIOTEST_TESTBOOKLIB_PARSER_TESTBOOKPARSER_HPP
