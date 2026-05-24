#include "LogFile.hpp"

#include <fstream>

namespace automationtest::utilities::tools {

namespace {
std::string g_filename = "c:\\temp\\templog.txt";

std::string JoinFragments(const std::vector<std::optional<std::string>>& fragments)
{
    std::string result;
    for (const auto& fragment : fragments) {
        if (fragment.has_value()) {
            result += *fragment;
        }
    }
    return result;
}

bool HasNoFragments(const std::vector<std::optional<std::string>>& fragments)
{
    for (const auto& fragment : fragments) {
        if (fragment.has_value()) {
            return false;
        }
    }
    return true;
}
}

std::string LogFile::Filename()
{
    return g_filename;
}

std::string LogFile::SetLoggerFilename(const std::string& filename)
{
    const std::string old = g_filename;
    g_filename = filename;
    return old;
}

void LogFile::WriteLog(const std::vector<std::optional<std::string>>& fragments)
{
    if (HasNoFragments(fragments)) {
        return;
    }

    Append(JoinFragments(fragments));
}

void LogFile::WriteLogLine(const std::vector<std::optional<std::string>>& fragments)
{
    if (HasNoFragments(fragments)) {
        Append("\n");
        return;
    }

    Append(JoinFragments(fragments) + '\n');
}

void LogFile::Append(const std::string& text)
{
    std::ofstream stream(g_filename, std::ios::app | std::ios::binary);
    stream << text;
}

} // namespace automationtest::utilities::tools
