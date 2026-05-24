#ifndef AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_KEYVALUELOOP_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_KEYVALUELOOP_HPP

#include "../Action/BlockAction.hpp"

namespace automationtest::testbooklib::flowcontrols {

class KeyValueLoop : public action::BlockAction {
public:
    KeyValueLoop(int line_number, const std::string& text);
    void Parse(const std::vector<std::string>& parts) override;
    void PlayActions(const ActionRunner& runner) override;

protected:
    virtual std::vector<std::string> GetList() const = 0;

    std::string name_only_variable_without_dollar_ {};
    std::string full_path_name_variable_without_dollar_ {};
    std::string parent_folder_name_ {};
    bool error_continue_on_next_ {false};
    bool recursive_ {false};
    std::string file_pattern_ {};
};

} // namespace automationtest::testbooklib::flowcontrols

#endif // AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_KEYVALUELOOP_HPP
