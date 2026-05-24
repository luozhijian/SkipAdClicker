#ifndef AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_IENUMERABLELOOP_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_IENUMERABLELOOP_HPP

#include "../Action/BlockAction.hpp"

namespace automationtest::testbooklib::flowcontrols {

class IEnumerableLoop : public action::BlockAction {
public:
    IEnumerableLoop(int line_number, const std::string& text);
    void Parse(const std::vector<std::string>& parts) override;
    void PlayActions(const ActionRunner& runner) override;

protected:
    virtual void SetVariable(const std::any& value);

    std::string input_variable_ {};
    std::string loop_variable_ {};
};

} // namespace automationtest::testbooklib::flowcontrols

#endif // AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_IENUMERABLELOOP_HPP
