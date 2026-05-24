#include "ForLinesInFile.hpp"

#include "../../Utilities/Exceptions/TestBreakException.hpp"
#include "../../Utilities/Exceptions/TestContinueException.hpp"

#include <fstream>

namespace automationtest::testbooklib::flowcontrols {

using automationtest::utilities::exceptions::TestBreakException;
using automationtest::utilities::exceptions::TestContinueException;

ForLinesInFile::ForLinesInFile(int line_number, const std::string& text)
    : action::BlockAction(line_number, text)
{
}

void ForLinesInFile::Parse(const std::vector<std::string>& parts)
{
    if (parts.size() >= 5) {
        variable_name_without_dollar_ = parts[1];
        ignore_empty_line_ = parts.size() > 3 ? AnyToBool(std::any(parts[3])).value_or(true) : true;
        resource_file_name_ = parts[4];
    }
}

void ForLinesInFile::PlayActions(const ActionRunner& runner)
{
    if (variable_service == nullptr || resource_file_name_.empty()) {
        return;
    }

    std::ifstream input(resource_file_name_);
    std::string line {};
    while (std::getline(input, line)) {
        if (ignore_empty_line_ && Trim(line).empty()) {
            continue;
        }
        variable_service->SetObject("$" + variable_name_without_dollar_, line);
        try {
            action::BlockAction::PlayActions(runner);
        } catch (const TestContinueException&) {
            continue;
        } catch (const TestBreakException&) {
            break;
        }
    }
}

} // namespace automationtest::testbooklib::flowcontrols
