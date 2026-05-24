#ifndef AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_DOWHILELOOP_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_DOWHILELOOP_HPP

#include "../Action/BlockAction.hpp"

namespace automationtest::testbooklib::flowcontrols {

class DoWhileLoop : public action::BlockAction {
public:
    DoWhileLoop(int line_number, const std::string& text);
    void Parse(const std::vector<std::string>& parts) override;
    void PlayActions(const ActionRunner& runner) override;

private:
    double sleep_seconds_ {-1.0};
    double total_seconds_ {-1.0};
    std::string loop_variable_ {};
};

} // namespace automationtest::testbooklib::flowcontrols

#endif // AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_DOWHILELOOP_HPP
