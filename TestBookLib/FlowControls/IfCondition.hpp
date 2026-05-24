#ifndef AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_IFCONDITION_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_IFCONDITION_HPP

#include "../Action/BlockAction.hpp"

namespace automationtest::testbooklib::flowcontrols {

class IfCondition : public action::BlockAction {
public:
    IfCondition(int line_number, const std::string& text);
    void Parse(const std::vector<std::string>& parts) override;
    void PlayActions(const ActionRunner& runner) override;

private:
    bool CheckConditionIsTrue() const;

    bool is_not_condition_ {false};
    std::string condition_ {};
};

} // namespace automationtest::testbooklib::flowcontrols

#endif // AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_IFCONDITION_HPP
