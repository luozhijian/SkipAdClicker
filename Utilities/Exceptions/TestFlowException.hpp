#ifndef AUTOMATIOTEST_EXCEPTIONS_TESTFLOWEXCEPTION_HPP
#define AUTOMATIOTEST_EXCEPTIONS_TESTFLOWEXCEPTION_HPP

#include "TestException.hpp"

namespace automationtest::utilities::exceptions {

class TestFlowException : public TestException {
public:
    TestFlowException();
    explicit TestFlowException(const std::string& message);
};

} // namespace automationtest::utilities::exceptions

#endif // AUTOMATIOTEST_EXCEPTIONS_TESTFLOWEXCEPTION_HPP
