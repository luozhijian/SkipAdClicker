#ifndef AUTOMATIOTEST_EXCEPTIONS_TESTCANCELLINGEXCEPTION_HPP
#define AUTOMATIOTEST_EXCEPTIONS_TESTCANCELLINGEXCEPTION_HPP

#include "TestFlowException.hpp"

namespace automationtest::utilities::exceptions {

class TestCancellingException : public TestFlowException {
public:
    TestCancellingException();
};

} // namespace automationtest::utilities::exceptions

#endif // AUTOMATIOTEST_EXCEPTIONS_TESTCANCELLINGEXCEPTION_HPP
