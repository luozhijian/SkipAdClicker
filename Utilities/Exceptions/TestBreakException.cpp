#include "TestBreakException.hpp"

namespace automationtest::utilities::exceptions {

TestBreakException::TestBreakException()
    : TestFlowException("TestBreakException")
{
}

TestBreakException::TestBreakException(const std::string& message)
    : TestFlowException(message)
{
}


void TestBreakException::Break()
{
    throw TestBreakException();
}

} // namespace automationtest::utilities::exceptions
