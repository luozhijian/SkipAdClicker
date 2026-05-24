#ifndef AUTOMATIOTEST_EXCEPTIONS_TESTEXCEPTION_HPP
#define AUTOMATIOTEST_EXCEPTIONS_TESTEXCEPTION_HPP

#include <stdexcept>
#include <string>

namespace automationtest::utilities::exceptions {

class TestException : public std::runtime_error {
public:
    TestException();
    explicit TestException(const std::string& message);
    TestException(const std::string& message, const std::exception& inner_exception);
};

} // namespace automationtest::utilities::exceptions

#endif // AUTOMATIOTEST_EXCEPTIONS_TESTEXCEPTION_HPP
