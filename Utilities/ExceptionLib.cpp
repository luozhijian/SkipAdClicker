#include "ExceptionLib.hpp"
#include <exception>
#include <string>
#include <stdexcept>

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


std::string ExceptionLib::exception_to_string() {
    auto ep = std::current_exception();
    if (!ep) {
        return "No exception active";
    }
    
    try {
        // Rethrow the captured pointer to analyze its type
        std::rethrow_exception(ep);
    } 
    catch (const std::exception& e) {
        // Captures all standard exceptions (std::runtime_error, std::out_of_range, etc.)
        return std::string("Standard exception: ") + e.what();
    } 
    catch (const std::string& e) {
        // Captures literal std::string throws
        return "String exception: " + e;
    } 
    catch (const char* e) {
        // Captures C-string literal throws (e.g., throw "Error")
        return std::string("C-string exception: ") + e;
    } 
    catch (int e) {
        // Captures primitive int throws
        return "Integer exception code: " + std::to_string(e);
    } 
    catch (...) {
        // Fallback for custom or unrecognized types
        return "Unknown type thrown";
    }
}

} // namespace automationtest::utilities
