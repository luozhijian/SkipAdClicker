#ifndef AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_DICTIONARYLOOP_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_DICTIONARYLOOP_HPP

#include "IEnumerableLoop.hpp"

namespace automationtest::testbooklib::flowcontrols {

class DictionaryLoop : public IEnumerableLoop {
public:
    DictionaryLoop(int line_number, const std::string& text);
};

} // namespace automationtest::testbooklib::flowcontrols

#endif // AUTOMATIOTEST_TESTBOOKLIB_FLOWCONTROLS_DICTIONARYLOOP_HPP
