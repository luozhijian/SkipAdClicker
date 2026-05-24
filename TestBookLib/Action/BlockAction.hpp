#ifndef AUTOMATIOTEST_TESTBOOKLIB_ACTION_BLOCKACTION_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_ACTION_BLOCKACTION_HPP

#include "../TestBookLibCommon.hpp"
#include "TestAction.hpp"
#include "../../Utilities/Types/EnumOnExcpetionAction.hpp"

#include <chrono>
#include <memory>
#include <vector>

namespace automationtest::testbooklib::action {

class BlockAction : public TestAction {
public:
    BlockAction(int line_number, std::string text);

    void Add(std::unique_ptr<TestAction> action);
    virtual void PlayActions(const ActionRunner& runner);
    virtual void Parse(const std::vector<std::string>& parts) = 0;

    [[nodiscard]] std::vector<std::unique_ptr<TestAction>>& ChildActions() noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<TestAction>>& ChildActions() const noexcept;

protected:
    VariableService* variable_service {nullptr};

public:
    int actions_count_in_loop {0};
    std::chrono::system_clock::time_point start_time {};
    automationtest::utilities::types::EnumOnExcpetionAction on_exception_action {automationtest::utilities::types::EnumOnExcpetionAction::OnErrorRaiseError};

private:
    std::vector<std::unique_ptr<TestAction>> child_actions_ {};
};

} // namespace automationtest::testbooklib::action

#endif // AUTOMATIOTEST_TESTBOOKLIB_ACTION_BLOCKACTION_HPP
