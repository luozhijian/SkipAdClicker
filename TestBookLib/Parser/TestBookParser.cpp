#include "TestBookParser.hpp"

#include "../TestBookLibCommon.hpp"
#include "TestPageParser.hpp"

#include <filesystem>

namespace automationtest::testbooklib::parser {

TestBook TestBookParser::Parse(const std::string& path, const std::string& filename)
{
    TestBook result {};
    if (!std::filesystem::exists(path)) {
        return result;
    }

    TestPageParser page_parser {};
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (!entry.is_regular_file()) {
            continue;
        }

        const auto full_name = entry.path().string();
        if (!filename.empty() && !EndsWithIgnoreCase(full_name, filename)) {
            continue;
        }
        if (EndsWithIgnoreCase(full_name, StaticResourceFileName)) {
            continue;
        }

        result.AddPage(page_parser.Parse(full_name));
    }

    return result;
}

} // namespace automationtest::testbooklib::parser
