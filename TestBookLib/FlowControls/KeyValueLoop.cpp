#include "KeyValueLoop.hpp"

#include "../../Utilities/Exceptions/TestContinueException.hpp"

#include <filesystem>

namespace automationtest::testbooklib::flowcontrols {

using automationtest::utilities::exceptions::TestContinueException;

KeyValueLoop::KeyValueLoop(int line_number, const std::string& text)
    : action::BlockAction(line_number, text)
{
}

void KeyValueLoop::Parse(const std::vector<std::string>& parts)
{
    if (parts.size() >= 7) {
        name_only_variable_without_dollar_ = parts[1];
        full_path_name_variable_without_dollar_ = parts[2];
        error_continue_on_next_ = AnyToBool(std::any(parts[3])).value_or(true);
        recursive_ = AnyToBool(std::any(parts[4])).value_or(false);
        file_pattern_ = parts[5];
        parent_folder_name_ = parts[6];
    }
}

void KeyValueLoop::PlayActions(const ActionRunner& runner)
{
    if (variable_service == nullptr) {
        return;
    }

    for (const auto& value : GetList()) {
        variable_service->SetObject("$" + full_path_name_variable_without_dollar_, value);
        variable_service->SetObject("$" + name_only_variable_without_dollar_, std::filesystem::path(value).filename().string());
        try {
            action::BlockAction::PlayActions(runner);
        } catch (const TestContinueException&) {
            continue;
        } catch (...) {
            if (!error_continue_on_next_) {
                throw;
            }
        }
    }
}

} // namespace automationtest::testbooklib::flowcontrols
