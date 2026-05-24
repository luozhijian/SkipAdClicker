#ifndef AUTOMATIOTEST_EXCEPTIONS_TESTFLOWSTOPEXCEPTION_HPP
#define AUTOMATIOTEST_EXCEPTIONS_TESTFLOWSTOPEXCEPTION_HPP

#include "TestException.hpp"

namespace automationtest::utilities::exceptions {

class TestFlowStopException : public TestException {
public:
    TestFlowStopException();
};

} // namespace automationtest::utilities::exceptions

#endif // AUTOMATIOTEST_EXCEPTIONS_TESTFLOWSTOPEXCEPTION_HPP
