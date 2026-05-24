#include "TestBreakException.hpp"

namespace automationtest::utilities::exceptions {

TestBreakException::TestBreakException()
    : TestFlowException("TestBreakException")
{
}

void TestBreakException::Break()
{
    throw TestBreakException();
}

} // namespace automationtest::utilities::exceptions
