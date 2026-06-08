#ifndef AUTOMATIOTEST_EXCEPTIONLIB_HPP
#define AUTOMATIOTEST_EXCEPTIONLIB_HPP

#include <functional>
#include <string>

namespace automationtest::utilities {

class ExceptionLib {
public:
    static bool WrapperExceptionAsWarn(const std::function<void()>& action) noexcept;
    static bool WrapperExceptionAsError(const std::function<void()>& action) noexcept;
    static bool WrapperExceptionAsInfo(const std::function<void()>& action) noexcept;
    static std::string exception_to_string() ;
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_EXCEPTIONLIB_HPP
