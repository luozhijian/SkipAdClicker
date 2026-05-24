#include "OnErrorResumeNextAction.hpp"

namespace automationtest::testbooklib::action {

OnErrorResumeNextAction::OnErrorResumeNextAction(int line_number, const std::string& text)
    : BlockAction(line_number, text)
{
}

void OnErrorResumeNextAction::Parse(const std::vector<std::string>& parts)
{
    (void)parts;
}

void OnErrorResumeNextAction::PlayActions(const ActionRunner& runner)
{
    for (auto& child : ChildActions()) {
        try {
            runner(*child);
        } catch (...) {
        }
    }
}

} // namespace automationtest::testbooklib::action
