#include "TestPageParser.hpp"

#include "../TestBookLibCommon.hpp"
#include "../../Utilities/Exceptions/TestException.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace automationtest::testbooklib::parser {

using automationtest::utilities::exceptions::TestException;

std::unique_ptr<TestPage> TestPageParser::Parse(const std::string& file_path)
{
    TestActionParser action_parser {};
    auto tree = ParseFileToTree(file_path);
    auto result = std::make_unique<TestPage>(0, file_path);
    ParseNodes(tree, *result, action_parser);
    return result;
}

std::vector<TestPageParser::ParsedNode> TestPageParser::ParseFileToTree(const std::string& file_path)
{
    std::ifstream input(file_path);
    if (!input) {
        throw TestException("Cannot open file " + file_path);
    }

    std::vector<ParsedNode> root {};
    std::vector<ParsedNode*> stack {};
    std::vector<int> indent_levels {};
    std::string line {};
    int row = 0;

    while (std::getline(input, line)) {
        ++row;
        if (Trim(line).empty()) {
            continue;
        }

        std::string normalized = line;
        std::string::size_type tab = 0;
        while ((tab = normalized.find('\t', tab)) != std::string::npos) {
            normalized.replace(tab, 1, "    ");
            tab += 4;
        }

        if (Trim(normalized).starts_with('#')) {
            continue;
        }

        int indentation = 0;
        while (indentation < static_cast<int>(normalized.size()) && normalized[static_cast<std::size_t>(indentation)] == ' ') {
            ++indentation;
        }
        if (indentation % 4 != 0) {
            throw TestException("Invalid indentation in line " + std::to_string(row) + ": " + line);
        }

        ParsedNode node {};
        node.line_number = row;
        node.text = normalized.substr(static_cast<std::size_t>(indentation));

        while (!indent_levels.empty() && indentation <= indent_levels.back()) {
            indent_levels.pop_back();
            stack.pop_back();
        }

        if (!indent_levels.empty() && indentation - indent_levels.back() != 4) {
            throw TestException("Invalid indentation jump in line " + std::to_string(row) + ": " + line);
        }

        if (stack.empty()) {
            root.push_back(std::move(node));
            stack.push_back(&root.back());
            indent_levels.push_back(indentation);
        } else {
            stack.back()->children.push_back(std::move(node));
            stack.push_back(&stack.back()->children.back());
            indent_levels.push_back(indentation);
        }
    }

    return root;
}

std::string TestPageParser::PrintTree(const std::vector<ParsedNode>& nodes, int indent_level)
{
    std::ostringstream output {};
    for (const auto& node : nodes) {
        output << std::string(static_cast<std::size_t>(indent_level) * 4, ' ') << node.text << '\n';
        output << PrintTree(node.children, indent_level + 1);
    }
    return output.str();
}

void TestPageParser::ParseNodes(const std::vector<ParsedNode>& nodes, action::BlockAction& block_action, TestActionParser& action_parser)
{
    for (const auto& node : nodes) {
        auto parsed_action = action_parser.Parse(node.line_number, node.text);
        auto* child_block = dynamic_cast<automationtest::testbooklib::action::BlockAction*>(parsed_action.get());
        if (!node.children.empty() && child_block == nullptr) {
            throw TestException("Line " + std::to_string(node.line_number) + " is not a block action: " + node.text);
        }

        if (child_block != nullptr) {
            ParseNodes(node.children, *child_block, action_parser);
        }
        block_action.Add(std::move(parsed_action));
    }
}

} // namespace automationtest::testbooklib::parser
