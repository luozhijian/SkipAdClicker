#ifndef AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_FORLINESINFILE_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_FORLINESINFILE_HPP

#include "../Action/BlockAction.hpp"

namespace automationtest::testbooklib::flowcontrols {

class ForLinesInFile : public action::BlockAction {
public:
    ForLinesInFile(int line_number, const std::string& text);
    void Parse(const std::vector<std::string>& parts) override;
    void PlayActions(const ActionRunner& runner) override;

private:
    std::string variable_name_without_dollar_ {};
    std::string resource_file_name_ {};
    bool ignore_empty_line_ {true};
};

} // namespace automationtest::testbooklib::flowcontrols

#endif // AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_FORLINESINFILE_HPP
