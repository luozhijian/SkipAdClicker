#include "DisposableStopWatch.hpp"

#include <sstream>

namespace automationtest::utilities {

DisposableStopWatch::DisposableStopWatch(std::function<void(const std::string&)> logger, std::string operation_name)
    : logger_(std::move(logger))
    , operation_name_(std::move(operation_name))
    , started_at_(std::chrono::steady_clock::now())
{
}

DisposableStopWatch::~DisposableStopWatch()
{
    if (!logger_) {
        return;
    }

    const auto elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - started_at_).count();
    std::ostringstream stream;
    stream << operation_name_ << " completed in " << elapsed << " seconds";
    logger_(stream.str());
}

} // namespace automationtest::utilities
