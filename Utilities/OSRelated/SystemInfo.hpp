#pragma once

#include <string>

namespace SystemInfo
{
    std::string GetUserName();
    std::string GetCurrentProcessName();
    std::string GetHostName();
    int GetProcessId();
    bool IsAnotherProcessWithSameNameRunning();
    bool KillAnotherProcessWithSameName();
}
