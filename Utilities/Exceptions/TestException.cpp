#include "TestException.hpp"

namespace automationtest::utilities::exceptions {

TestException::TestException()
    : std::runtime_error("TestException")
{
}

TestException::TestException(const std::string& message)
    : std::runtime_error(message)
{
}

TestException::TestException(const std::string& message, const std::exception& inner_exception)
    : std::runtime_error(message + ": " + inner_exception.what())
{
}

} // namespace automationtest::utilities::exceptions
