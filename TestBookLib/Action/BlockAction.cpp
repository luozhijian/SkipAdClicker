#include "BlockAction.hpp"

namespace automationtest::testbooklib::action {

BlockAction::BlockAction(int line_number, std::string text)
    : TestAction(line_number, std::move(text)),
      variable_service(GlobalVariableService())
{
}

void BlockAction::Add(std::unique_ptr<TestAction> action)
{
    child_actions_.push_back(std::move(action));
}

void BlockAction::PlayActions(const ActionRunner& runner)
{
    for (auto& child : child_actions_) {
        runner(*child);
    }
}

std::vector<std::unique_ptr<TestAction>>& BlockAction::ChildActions() noexcept
{
    return child_actions_;
}

const std::vector<std::unique_ptr<TestAction>>& BlockAction::ChildActions() const noexcept
{
    return child_actions_;
}

} // namespace automationtest::testbooklib::action
