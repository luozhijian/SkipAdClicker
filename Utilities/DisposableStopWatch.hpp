#ifndef AUTOMATIOTEST_DISPOSABLESTOPWATCH_HPP
#define AUTOMATIOTEST_DISPOSABLESTOPWATCH_HPP

#include <chrono>
#include <functional>
#include <string>

namespace automationtest::utilities {

class DisposableStopWatch {
public:
    DisposableStopWatch(std::function<void(const std::string&)> logger, std::string operation_name);
    ~DisposableStopWatch();

private:
    std::function<void(const std::string&)> logger_;
    std::string operation_name_;
    std::chrono::steady_clock::time_point started_at_;
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_DISPOSABLESTOPWATCH_HPP
