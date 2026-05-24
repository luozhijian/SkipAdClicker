#ifndef AUTOMATIOTEST_LOGGER_HPP
#define AUTOMATIOTEST_LOGGER_HPP

#include <functional>
#include <string>

namespace automationtest::utilities {

enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Error = 3,
    Off = 4
};

struct LogSettings {
    bool enabled {true};
    LogLevel minimum_level {LogLevel::Info};
    std::string file_path {};
    bool write_to_console {false};
};

class Logger {
public:
    static std::function<void(std::string)> log_to_view;
    static void Configure(const LogSettings& settings);
    static LogSettings Settings();
    static LogLevel GetLogLevel();
    static void SetLogToView(std::function<void(std::string)> callback);
    static void LogToView(const std::string& message);
    static bool IsEnable();
    static bool IsAboveDebug();
    static bool IsAboveInfo();
    static bool IsAboveError();

    static void Error(const std::string& message, const std::string& category = {});
    static void Info(const std::string& message, const std::string& category = {});
    static void Debug(const std::string& message, const std::string& category = {});
    static void Trace(const std::string& message, const std::string& category = {});


    static void ErrorMessage(const std::string& message)
    {
        Error( message, {});
    }
    static void InfoMessage(const std::string& message)
    {
        Info(message, {});
    }
    static void DebugMessage(const std::string& message)
    {
        Debug(message, {});
    }
    static void TraceMessage(const std::string& message)
    {
        Trace(message, {});
    }


    static LogLevel ParseLevel(const std::string& level, LogLevel default_level = LogLevel::Info);
    static std::string ToString(LogLevel level);

private:
    static void Write(LogLevel level, const std::string& message, const std::string& category);
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_LOGGER_HPP