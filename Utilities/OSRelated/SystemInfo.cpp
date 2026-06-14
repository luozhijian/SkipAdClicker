#include "SystemInfo.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>

#ifdef _WIN32

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <Lmcons.h>
#include <process.h>

#undef GetUserName

namespace SystemInfo
{
    std::string GetUserName()
    {
        char buffer[UNLEN + 1];
        DWORD size = UNLEN + 1;

        if (::GetUserNameA(buffer, &size))
        {
            return buffer;
        }

        return "unknown";
    }

    std::string GetCurrentProcessName()
    {
        char buffer[MAX_PATH];

        DWORD len = ::GetModuleFileNameA(nullptr, buffer, MAX_PATH);

        if (len == 0)
        {
            return "unknown";
        }

        return std::filesystem::path(buffer).filename().string();
    }

    std::string GetHostName()
    {
        char buffer[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD size = sizeof(buffer);

        if (::GetComputerNameA(buffer, &size))
        {
            return buffer;
        }

        return "unknown";
    }

    int GetProcessId()
    {
        return static_cast<int>(_getpid());
    }

    bool IsAnotherProcessWithSameNameRunning()
    {
        return true;
    }
}

#else

#include <unistd.h>
#include <pwd.h>
#include <limits.h>

namespace SystemInfo
{
    std::string GetUserName()
    {
        const char* login = getlogin();
        if (login)
        {
            return login;
        }

        passwd* pw = getpwuid(getuid());
        if (pw && pw->pw_name)
        {
            return pw->pw_name;
        }

        return "unknown";
    }

    std::string GetCurrentProcessName()
    {
        char buffer[PATH_MAX] = {};

        ssize_t len =
            readlink("/proc/self/exe",
                     buffer,
                     sizeof(buffer) - 1);

        if (len <= 0)
        {
            return "unknown";
        }

        buffer[len] = '\0';

        return std::filesystem::path(buffer).filename().string();
    }

    std::string GetHostName()
    {
        char buffer[HOST_NAME_MAX + 1] = {};

        if (gethostname(buffer, sizeof(buffer)) == 0)
        {
            return buffer;
        }

        return "unknown";
    }

    int GetProcessId()
    {
        return static_cast<int>(getpid());
    }

    bool IsAnotherProcessWithSameNameRunning()
    {
        const std::string myName = SystemInfo::GetCurrentProcessName();
        const int myPid = SystemInfo::GetProcessId();

        for (const auto& entry : std::filesystem::directory_iterator("/proc"))
        {
            if (!entry.is_directory())
                continue;

            const std::string pidStr = entry.path().filename().string();

            if (pidStr.empty() ||
                !std::all_of(pidStr.begin(), pidStr.end(), [](unsigned char ch) { return std::isdigit(ch); }))
            {
                continue;
            }

            int pid = std::stoi(pidStr);

            if (pid == myPid)
                continue;

            std::ifstream commFile(entry.path() / "comm");

            if (!commFile)
                continue;

            std::string processName;
            std::getline(commFile, processName);

            if (processName == myName)
            {
                return true;
            }
        }

        return false;
    }
}

#endif
