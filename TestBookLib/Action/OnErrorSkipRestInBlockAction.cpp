#include "OnErrorSkipRestInBlockAction.hpp"

namespace automationtest::testbooklib::action {

OnErrorSkipRestInBlockAction::OnErrorSkipRestInBlockAction(int line_number, const std::string& text)
    : BlockAction(line_number, text)
{
}

void OnErrorSkipRestInBlockAction::Parse(const std::vector<std::string>& parts)
{
    (void)parts;
}

void OnErrorSkipRestInBlockAction::PlayActions(const ActionRunner& runner)
{
    for (auto& child : ChildActions()) {
        try {
            runner(*child);
        } catch (...) {
            break;
        }
    }
}

} // namespace automationtest::testbooklib::action
