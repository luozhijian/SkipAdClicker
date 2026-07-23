#include "TestContinueException.hpp"

namespace automationtest::utilities::exceptions {

TestContinueException::TestContinueException()
    : TestFlowException("TestContinueException")
{
}


void TestContinueException::ThrowTestContinueException()
{
    throw TestContinueException();
}

} // namespace automationtest::utilities::exceptions
