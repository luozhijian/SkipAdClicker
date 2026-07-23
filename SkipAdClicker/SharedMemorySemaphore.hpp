// SharedMemorySemaphore.hpp
#pragma once

#ifdef _WIN32
using SharedSemaphoreHandle = void*;
using SharedMemoryHandle = void*;
#else
#include <semaphore.h>
#endif

#include <cstdint>
#include <stop_token>
#include <string>
#include <thread>
namespace automationtest::app {
    class MainWindow;

    void ClearSharedMemorySemaphore();
class SharedMemorySemaphore
{
public:
    explicit SharedMemorySemaphore( const std::string& processName)
    : name_( BuildName( processName) )
    {
    }
    
    ~SharedMemorySemaphore()
    {
        Close();
        // Clear();
    }

    SharedMemorySemaphore(const SharedMemorySemaphore&) = delete;
    SharedMemorySemaphore& operator=(const SharedMemorySemaphore&) = delete;

    const std::string& Name() const
    {
        return name_;
    }

    // Check whether shared memory semaphore exists
    bool Exists() const;
    // Clear / remove shared memory semaphore
    void Clear();
    // Creator creates semaphore and waits for consumer trigger
    bool CreateForCreator(const std::string& version, unsigned int initialValue = 0);
    // Consumer opens existing semaphore
    void OpenForConsumer();
    std::string RunningVersion() const;
    // Creator waits
    void Wait( MainWindow * mainWin);
     // Consumer triggers creator
    void Trigger();
    void RequestQuit();
    void Close();
private:
    struct SharedSemaphoreState
    {
#ifndef _WIN32
        sem_t semaphore;
#endif
        std::uint32_t initialized;
        std::uint32_t command;
        char version[32];
    };

    static std::string BuildName(const std::string& processName);
    static std::string BuildStateName(const std::string& semaphoreName);
    void SetCommand(std::uint32_t command);
    void Signal();
    void StopThreadToWait();
    void StartThreadToWait();
#ifndef _WIN32
    bool MapSharedMemory(int fd);
    bool WaitUntilInitialized(int timeoutMs) const;
#endif
    bool WaitForSignal(int timeoutMs);
    void RealWait(std::stop_token stoken);

private:
    std::string name_;
#ifdef _WIN32
    SharedSemaphoreHandle semaphore_ = nullptr;
    SharedMemoryHandle shared_memory_ = nullptr;
    SharedSemaphoreState* state_ = nullptr;
#else
    int shm_fd_ = -1;
    SharedSemaphoreState* state_ = nullptr;
#endif
    MainWindow * mainWin = nullptr;
    std::jthread *  waiting_thread = nullptr;
};

extern   SharedMemorySemaphore* g_SharedMemorySemaphore;
};
