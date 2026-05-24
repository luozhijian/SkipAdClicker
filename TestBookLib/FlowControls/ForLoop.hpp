#ifndef AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_FORLOOP_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_FORLOOP_HPP

#include "../Action/BlockAction.hpp"

namespace automationtest::testbooklib::flowcontrols {

class ForLoop : public action::BlockAction {
public:
    ForLoop(int line_number, const std::string& text);
    void Parse(const std::vector<std::string>& parts) override;
    void PlayActions(const ActionRunner& runner) override;

private:
    double sleep_seconds_ {-1.0};
    double total_seconds_ {-1.0};
    std::string loop_variable_ {};
};

} // namespace automationtest::testbooklib::flowcontrols

#endif // AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_FORLOOP_HPP
