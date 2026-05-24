#include "TestActionParser.hpp"

#include "../TestBookLibCommon.hpp"

namespace automationtest::testbooklib::parser {

std::unique_ptr<TestAction> TestActionParser::Parse(int line_number, const std::string& action_line)
{
    if (auto let_result = ProcessLetIfItIsLet(line_number, action_line)) {
        return let_result;
    }

    const auto parts = SplitActionLine(action_line);
    auto result = action::ActionFactory::CreateAction(parts, action_line, line_number);

    if (!parts.empty()) {
        const auto separator = parts.front().find('=');
        if (separator == std::string::npos) {
            result->action = parts.front();
        } else {
            result->return_value = parts.front().substr(0, separator);
            result->action = parts.front().substr(separator + 1);
        }
    }

    for (std::size_t index = 1; index < parts.size(); ++index) {
        result->parameters.emplace_back(parts[index]);
    }
    return result;
}

std::unique_ptr<TestAction> TestActionParser::ProcessLetIfItIsLet(int line_number, const std::string& action_line)
{
    const auto trimmed = Trim(action_line);
    if (!StartsWithIgnoreCase(trimmed, "let")) {
        return {};
    }

    const auto equal_sign = trimmed.find('=');
    if (equal_sign == std::string::npos) {
        return {};
    }

    auto result = std::make_unique<TestAction>(line_number, action_line);
    result->action = "let";
    result->return_value = Trim(trimmed.substr(3, equal_sign - 3));
    result->parameters.emplace_back(trimmed.substr(equal_sign + 1));
    return result;
}

} // namespace automationtest::testbooklib::parser
