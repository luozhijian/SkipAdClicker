#ifndef AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_DEFFUNCTION_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_DEFFUNCTION_HPP

#include "../Action/BlockAction.hpp"
#include "../Parameter/SelfDefinedFunctionParameters.hpp"

#include <any>

namespace automationtest::testbooklib::flowcontrols {

class DefFunction : public action::BlockAction {
public:
    DefFunction(int line_number, const std::string& text);
    void Parse(const std::vector<std::string>& parts) override;
    void PlayActions(const ActionRunner& runner) override;

    [[nodiscard]] const std::string& Name() const noexcept;
    bool AddResultIfInRange(const std::string& key, std::any value);
    [[nodiscard]] parameter::SelfDefinedFunctionParameters& Parameters() noexcept;
    [[nodiscard]] const parameter::SelfDefinedFunctionParameters& Parameters() const noexcept;

private:
    std::string name_ {};
    parameter::SelfDefinedFunctionParameters parameters_ {};
};

} // namespace automationtest::testbooklib::flowcontrols

#endif // AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_DEFFUNCTION_HPP
