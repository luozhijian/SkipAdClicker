#include "ExceptionLib.hpp"

namespace automationtest::utilities {

bool ExceptionLib::WrapperExceptionAsWarn(const std::function<void()>& action) noexcept
{
    try {
        action();
        return true;
    } catch (...) {
        return false;
    }
}

bool ExceptionLib::WrapperExceptionAsError(const std::function<void()>& action) noexcept
{
    return WrapperExceptionAsWarn(action);
}

bool ExceptionLib::WrapperExceptionAsInfo(const std::function<void()>& action) noexcept
{
    return WrapperExceptionAsWarn(action);
}

} // namespace automationtest::utilities
