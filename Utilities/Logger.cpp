#include "Logger.hpp"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <utility>

namespace automationtest::utilities {

std::function<void(std::string)> Logger::log_to_view {};

namespace {

std::mutex log_mutex;
LogSettings current_settings {};

std::string Trim(std::string value)
{
    const auto first = std::find_if_not(value.begin(), value.end(), [](unsigned char ch) { return std::isspace(ch) != 0; });
    const auto last = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char ch) { return std::isspace(ch) != 0; }).base();
    if (first >= last) {
        return {};
    }
    return std::string(first, last);
}

std::string ToLower(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    return value;
}

std::string Timestamp()
{
    const auto now = std::chrono::system_clock::now();
    const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    const std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm local {};
#if defined(_WIN32)
    localtime_s(&local, &time);
#else
    localtime_r(&time, &local);
#endif

    std::ostringstream stream;
    stream << std::put_time(&local, "%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << milliseconds.count();
    return stream.str();
}

bool ShouldWrite(LogLevel level)
{
    return current_settings.enabled && level >= current_settings.minimum_level && current_settings.minimum_level != LogLevel::Off;
}

} // namespace

void Logger::Configure(const LogSettings& settings)
{
    std::lock_guard lock(log_mutex);
    current_settings = settings;
}

LogSettings Logger::Settings()
{
    std::lock_guard lock(log_mutex);
    return current_settings;
}
LogLevel Logger::GetLogLevel()
{
    std::lock_guard lock(log_mutex);
    return current_settings.minimum_level;
}
void Logger::SetLogToView(std::function<void(std::string)> callback)
{
    std::lock_guard lock(log_mutex);
    log_to_view = std::move(callback);
}

void Logger::LogToView(const std::string& message)
{
    std::function<void(std::string)> callback;
    {
        std::lock_guard lock(log_mutex);
        callback = log_to_view;
    }

    if (callback) {
        callback(message);
    }
}

bool Logger::IsEnable()
{
    std::lock_guard lock(log_mutex);
    return current_settings.enabled;
}

bool Logger::IsAboveDebug()
{
    std::lock_guard lock(log_mutex);
    return current_settings.enabled && current_settings.minimum_level <= LogLevel::Debug;
}

bool Logger::IsAboveInfo()
{
    std::lock_guard lock(log_mutex);
    return current_settings.enabled && current_settings.minimum_level <= LogLevel::Info;
}

bool Logger::IsAboveError()
{
    std::lock_guard lock(log_mutex);
    return current_settings.enabled && current_settings.minimum_level <= LogLevel::Error;
}

void Logger::Error(const std::string& message, const std::string& category)
{
    Write(LogLevel::Error, message, category);
}

void Logger::Info(const std::string& message, const std::string& category)
{
    Write(LogLevel::Info, message, category);
}

void Logger::Debug(const std::string& message, const std::string& category)
{
    Write(LogLevel::Debug, message, category);
}

void Logger::Trace(const std::string& message, const std::string& category)
{
    Write(LogLevel::Trace, message, category);
}

LogLevel Logger::ParseLevel(const std::string& level, LogLevel default_level)
{
    const auto normalized = ToLower(Trim(level));
    if (normalized == "trace") {
        return LogLevel::Trace;
    }
    if (normalized == "debug") {
        return LogLevel::Debug;
    }
    if (normalized == "info" || normalized == "information") {
        return LogLevel::Info;
    }
    if (normalized == "error") {
        return LogLevel::Error;
    }
    if (normalized == "off" || normalized == "none" || normalized == "disabled") {
        return LogLevel::Off;
    }
    return default_level;
}

std::string Logger::ToString(LogLevel level)
{
    switch (level) {
    case LogLevel::Trace:
        return "TRACE";
    case LogLevel::Debug:
        return "DEBUG";
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Error:
        return "ERROR";
    case LogLevel::Off:
        return "OFF";
    }
    return "INFO";
}

void Logger::Write(LogLevel level, const std::string& message, const std::string& category)
{
    std::lock_guard lock(log_mutex);
    if (!ShouldWrite(level)) {
        return;
    }

    std::ostringstream line;
    line << Timestamp() << " [" << ToString(level) << "]";
    if (!category.empty()) {
        line << " [" << category << "]";
    }
    line << ' ' << message;
    const auto text = line.str();

    if (current_settings.write_to_console) {
        std::clog << text << std::endl;
    }

    if (!current_settings.file_path.empty()) {
        const auto parent_path = std::filesystem::path(current_settings.file_path).parent_path();
        if (!parent_path.empty()) {
            std::filesystem::create_directories(parent_path);
        }
        std::ofstream output(current_settings.file_path, std::ios::app);
        if (output) {
            output << text << '\n';
        }
    }
}

} // namespace automationtest::utilities
