#include "TestFlowException.hpp"

namespace automationtest::utilities::exceptions {

TestFlowException::TestFlowException()
    : TestException("TestFlowException")
{
}

TestFlowException::TestFlowException(const std::string& message)
    : TestException(message)
{
}

} // namespace automationtest::utilities::exceptions
