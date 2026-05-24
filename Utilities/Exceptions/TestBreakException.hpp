#ifndef AUTOMATIOTEST_EXCEPTIONS_TESTBREAKEXCEPTION_HPP
#define AUTOMATIOTEST_EXCEPTIONS_TESTBREAKEXCEPTION_HPP

#include "TestFlowException.hpp"

namespace automationtest::utilities::exceptions {

class TestBreakException : public TestFlowException {
public:
    TestBreakException();
    static void Break();
};

} // namespace automationtest::utilities::exceptions

#endif // AUTOMATIOTEST_EXCEPTIONS_TESTBREAKEXCEPTION_HPP
