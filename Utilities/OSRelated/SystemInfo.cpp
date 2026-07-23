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
#include <tlhelp32.h>

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

    bool KillAnotherProcessWithSameName()
    {
        const std::string current_process_name = GetCurrentProcessName();
        const DWORD current_process_id = ::GetCurrentProcessId();
        HANDLE snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE)
            return false;

        PROCESSENTRY32 process_entry{};
        process_entry.dwSize = sizeof(process_entry);
        bool killed = false;

        if (::Process32First(snapshot, &process_entry))
        {
            do
            {
                if (process_entry.th32ProcessID == current_process_id
                    || std::filesystem::path(process_entry.szExeFile).filename().string()
                        != current_process_name)
                {
                    continue;
                }

                HANDLE process = ::OpenProcess(PROCESS_TERMINATE, FALSE, process_entry.th32ProcessID);
                if (process != nullptr)
                {
                    killed = ::TerminateProcess(process, 1) != FALSE;
                    ::CloseHandle(process);
                }
                break;
            } while (::Process32Next(snapshot, &process_entry));
        }

        ::CloseHandle(snapshot);
        return killed;
    }
}

#else

#include <unistd.h>
#include <pwd.h>
#include <limits.h>
#include <csignal>

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

    bool KillAnotherProcessWithSameName()
    {
        const std::string current_process_name = GetCurrentProcessName();
        const int current_process_id = GetProcessId();

        for (const auto& entry : std::filesystem::directory_iterator("/proc"))
        {
            const std::string pid_text = entry.path().filename().string();
            if (!entry.is_directory()
                || pid_text.empty()
                || !std::all_of(
                    pid_text.begin(), pid_text.end(),
                    [](unsigned char character) { return std::isdigit(character); }))
            {
                continue;
            }

            const int process_id = std::stoi(pid_text);
            if (process_id == current_process_id)
                continue;

            std::error_code error;
            const auto executable = std::filesystem::read_symlink(entry.path() / "exe", error);
            if (error || executable.filename().string() != current_process_name)
                continue;

            return ::kill(process_id, SIGKILL) == 0;
        }

        return false;
    }
}

#endif
