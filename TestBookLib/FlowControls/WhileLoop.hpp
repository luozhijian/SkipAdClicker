#ifndef AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_WHILELOOP_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_WHILELOOP_HPP

#include "../Action/BlockAction.hpp"

#include <optional>

namespace automationtest::testbooklib::flowcontrols {

class WhileLoop : public action::BlockAction {
public:
    WhileLoop(int line_number, const std::string& text);
    void Parse(const std::vector<std::string>& parts) override;
    void PlayActions(const ActionRunner& runner) override;

private:
    void ParseAtRuntime(const std::vector<std::string>& parts);

    std::vector<std::string> raw_parts_ {};
    double sleep_seconds_ {-1.0};
    double total_seconds_ {-1.0};
    std::string loop_variable_ {};
    std::optional<int> max_error_count_ {};
    int current_error_count_ {0};
};

} // namespace automationtest::testbooklib::flowcontrols

#endif // AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_WHILELOOP_HPP
