#ifndef AUTOMATIOTEST_EXCEPTIONS_TESTCONTINUEEXCEPTION_HPP
#define AUTOMATIOTEST_EXCEPTIONS_TESTCONTINUEEXCEPTION_HPP

#include "TestFlowException.hpp"

namespace automationtest::utilities::exceptions {

class TestContinueException : public TestFlowException {
public:
    TestContinueException();
    static void ThrowTestContinueException();
};

} // namespace automationtest::utilities::exceptions

#endif // AUTOMATIOTEST_EXCEPTIONS_TESTCONTINUEEXCEPTION_HPP
