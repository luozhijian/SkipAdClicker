#ifndef AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_FORSUBFOLDERSINFOLDER_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_FORSUBFOLDERSINFOLDER_HPP

#include "KeyValueLoop.hpp"

namespace automationtest::testbooklib::flowcontrols {

class ForSubFoldersInFolder : public KeyValueLoop {
public:
    ForSubFoldersInFolder(int line_number, const std::string& text);

protected:
    std::vector<std::string> GetList() const override;
};

} // namespace automationtest::testbooklib::flowcontrols

#endif // AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_FORSUBFOLDERSINFOLDER_HPP
