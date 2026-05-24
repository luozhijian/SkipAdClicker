#ifndef AUTOMATIOTEST_TOOLS_LOGFILE_HPP
#define AUTOMATIOTEST_TOOLS_LOGFILE_HPP

#include <optional>
#include <string>
#include <vector>

namespace automationtest::utilities::tools {

class LogFile {
public:
    static std::string Filename();
    static std::string SetLoggerFilename(const std::string& filename);
    static void WriteLog(const std::vector<std::optional<std::string>>& fragments = {});
    static void WriteLogLine(const std::vector<std::optional<std::string>>& fragments = {});

private:
    static void Append(const std::string& text);
};

} // namespace automationtest::utilities::tools

#endif // AUTOMATIOTEST_TOOLS_LOGFILE_HPP
