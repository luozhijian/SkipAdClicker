// SharedMemorySemaphore.hpp
#pragma once

#include <semaphore.h>

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
    void CreateForCreator(unsigned int initialValue = 0);
    // Consumer opens existing semaphore
    void OpenForConsumer();
    // Creator waits
    void Wait( MainWindow * mainWin);
     // Consumer triggers creator
    void Trigger();
    void Close();
private:
    struct SharedSemaphoreState
    {
        sem_t semaphore;
        std::uint32_t initialized;
    };

    static std::string BuildName(const std::string& processName);
    void StopThreadToWait();
    void StartThreadToWait();
    bool MapSharedMemory(int fd);
    bool WaitUntilInitialized(int timeoutMs) const;
    bool WaitForSignal(int timeoutMs);
    void RealWait(std::stop_token stoken);

private:
    std::string name_;
    int shm_fd_ = -1;
    SharedSemaphoreState* state_ = nullptr;
    MainWindow * mainWin = nullptr;
    std::jthread *  waiting_thread = nullptr;
};

extern   SharedMemorySemaphore* g_SharedMemorySemaphore;
};
