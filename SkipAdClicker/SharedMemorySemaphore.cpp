// SharedMemorySemaphore.hpp

#include <chrono>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <sys/mman.h>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>

#include  "SharedMemorySemaphore.hpp"
#include  "MainWindow.hpp"
#include  "../Utilities/Logger.hpp"
#include  "../Utilities/OSRelated/SystemInfo.hpp"
namespace automationtest::app {

namespace {
    constexpr std::uint32_t kInitializedMagic = 0x534d5348; // "SMSH"
}

 SharedMemorySemaphore* g_SharedMemorySemaphore=nullptr;
    void ClearSharedMemorySemaphore()
    {
        if ( g_SharedMemorySemaphore)
        {
            g_SharedMemorySemaphore->Close();
            if ( g_SharedMemorySemaphore->Exists() )
            {
                g_SharedMemorySemaphore->Clear();
            }
            delete g_SharedMemorySemaphore;
            g_SharedMemorySemaphore = nullptr;
        }
    }

    // Check whether shared memory semaphore exists
    bool SharedMemorySemaphore::Exists() const
    {
        int fd = shm_open(name_.c_str(), O_RDWR, 0);
        if (fd == -1)
        {
            if (errno == ENOENT)
                return false;

            automationtest::utilities::Logger::Error(
                "shm_open failed in Exists(): " + name_ + " : " + std::string(std::strerror(errno)) );
            return false;
        }

        close(fd);
        return true;
    }

    // Clear / remove shared memory semaphore
    void SharedMemorySemaphore::Clear()
    {
        StopThreadToWait();

        SharedSemaphoreState* mappedState = state_;
        int temporaryFd = -1;
        bool temporaryMapping = false;

        if (mappedState == nullptr)
        {
            temporaryFd = shm_open(name_.c_str(), O_RDWR, 0);
            if (temporaryFd != -1)
            {
                void* memory = mmap(nullptr, sizeof(SharedSemaphoreState), PROT_READ | PROT_WRITE, MAP_SHARED, temporaryFd, 0);
                if (memory == MAP_FAILED)
                {
                    automationtest::utilities::Logger::Error(
                        "mmap failed in Clear(): " + name_ + " : " + std::string(std::strerror(errno)));
                }
                else
                {
                    mappedState = static_cast<SharedSemaphoreState*>(memory);
                    temporaryMapping = true;
                }
            }
            else if (errno != ENOENT)
            {
                automationtest::utilities::Logger::Error(
                    "shm_open failed in Clear(): " + name_ + " : " + std::string(std::strerror(errno)));
            }
        }

        if (mappedState != nullptr && mappedState->initialized == kInitializedMagic)
        {
            if (sem_destroy(&mappedState->semaphore) == -1)
            {
                automationtest::utilities::Logger::Error(
                    "sem_destroy failed in Clear(): " + name_ + " : " + std::string(std::strerror(errno)));
            }
            mappedState->initialized = 0;
        }

        if (temporaryMapping)
        {
            munmap(mappedState, sizeof(SharedSemaphoreState));
        }
        if (temporaryFd != -1)
        {
            close(temporaryFd);
        }

        if (shm_unlink(name_.c_str()) == -1)
        {
            if (errno != ENOENT)
            {
                automationtest::utilities::Logger::Error(
                    "shm_unlink failed: " + name_ + " : " + std::string(std::strerror(errno)));
            }
        }

        if (state_ != nullptr)
        {
            Close();
        }
    }

    // Creator creates semaphore and ws for consumer trigger
    void SharedMemorySemaphore::CreateForCreator(unsigned int initialValue)
    {
        std::cout <<  "CreateForCreator";

        Close();
        shm_fd_ = shm_open(
            name_.c_str(),
            O_CREAT | O_EXCL | O_RDWR,
            0666);

        if (shm_fd_ == -1 && errno == EEXIST)
        {
            Clear();
            shm_fd_ = shm_open(
                name_.c_str(),
                O_CREAT | O_EXCL | O_RDWR,
                0666);
        }

        if (shm_fd_ == -1)
        {
            automationtest::utilities::Logger::Error(
                "shm_open failed in CreateForCreator(): " + name_ + " : " + std::string(std::strerror(errno)));
            std::cout <<    
                "shm_open failed in CreateForCreator(): " + std::string(std::strerror(errno));
            return;
        }

        if (ftruncate(shm_fd_, sizeof(SharedSemaphoreState)) == -1)
        {
            automationtest::utilities::Logger::Error(
                "ftruncate failed in CreateForCreator(): " + name_ + " : " + std::string(std::strerror(errno)));
            Close();
            Clear();
            return;
        }

        if (!MapSharedMemory(shm_fd_))
        {
            Close();
            Clear();
            return;
        }

        std::memset(state_, 0, sizeof(SharedSemaphoreState));
        if (sem_init(&state_->semaphore, 1, initialValue) == -1)
        {
            automationtest::utilities::Logger::Error(
                "sem_init failed in CreateForCreator(): " + name_ + " : " + std::string(std::strerror(errno)));
            Close();
            Clear();
            return;
        }

        state_->initialized = kInitializedMagic;
    }

    // Consumer opens existing semaphore
    void SharedMemorySemaphore::OpenForConsumer()
    {
        Close();

        shm_fd_ = shm_open(name_.c_str(), O_RDWR, 0);
        if (shm_fd_ == -1)
        {
            automationtest::utilities::Logger::Error(
                "shm_open failed in OpenForConsumer(): " + name_ + " : " + std::string(std::strerror(errno)));
            return;
        }

        if (!MapSharedMemory(shm_fd_))
        {
            Close();
            return;
        }

        if (!WaitUntilInitialized(1000))
        {
            automationtest::utilities::Logger::Error(
                "Shared memory semaphore is not initialized in OpenForConsumer(): " + name_);
            Close();
        }
    }

    // Creator waits
    void SharedMemorySemaphore::Wait(MainWindow * mainWin)
    {
        this->mainWin = mainWin;
        StartThreadToWait();
    }
    void SharedMemorySemaphore::StopThreadToWait()
    {
        if ( waiting_thread != nullptr )
        {
            waiting_thread->request_stop();
            waiting_thread->join();
            delete waiting_thread;
            waiting_thread = nullptr;
        }
    }
    void SharedMemorySemaphore::StartThreadToWait()
    {
        StopThreadToWait();
        waiting_thread = new std::jthread([this](std::stop_token stoken) {
            RealWait(stoken);
        });
    }

    bool SharedMemorySemaphore::MapSharedMemory(int fd)
    {
        void* memory = mmap(nullptr, sizeof(SharedSemaphoreState), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (memory == MAP_FAILED)
        {
            automationtest::utilities::Logger::Error(
                "mmap failed: " + name_ + " : " + std::string(std::strerror(errno)));
            return false;
        }

        state_ = static_cast<SharedSemaphoreState*>(memory);
        return true;
    }

    bool SharedMemorySemaphore::WaitUntilInitialized(int timeoutMs) const
    {
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);
        while (std::chrono::steady_clock::now() < deadline)
        {
            if (state_ != nullptr && state_->initialized == kInitializedMagic)
                return true;

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        return state_ != nullptr && state_->initialized == kInitializedMagic;
    }

    bool SharedMemorySemaphore::WaitForSignal(int timeoutMs)
    {
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);

        ts.tv_sec += timeoutMs / 1000;
        ts.tv_nsec += (timeoutMs % 1000) * 1000000;

        if (ts.tv_nsec >= 1000000000)
        {
            ts.tv_sec++;
            ts.tv_nsec -= 1000000000;
        }

        int rc = sem_timedwait(&state_->semaphore, &ts);

        if (rc == 0)
            return true;

        if (errno == ETIMEDOUT)
            return false;

        return false;
    }

    void SharedMemorySemaphore::RealWait(std::stop_token stoken)
    {
        if (!state_)
        {
            automationtest::utilities::Logger::Error ("Shared memory semaphore is not opened.");
            return;
        }

        while (!stoken.stop_requested()) 
        {
            if (state_ == nullptr ) 
                break;
            if ( WaitForSignal ( 200)  )
            {
                this->mainWin->ShowAndRestore();
            }
        }   

    }

    // Consumer triggers creator
    void SharedMemorySemaphore::Trigger()
    {
        if (!state_)
            throw std::runtime_error("Shared memory semaphore is not opened.");

        if (sem_post(&state_->semaphore) == -1)
        {
            throw std::runtime_error(
                "sem_post failed: " + std::string(std::strerror(errno)));
        }
    }

    void SharedMemorySemaphore::Close()
    {
        StopThreadToWait();

        if (state_)
        {
            munmap(state_, sizeof(SharedSemaphoreState));
            state_ = nullptr;
        }

        if (shm_fd_ != -1)
        {
            close(shm_fd_);
            shm_fd_ = -1;
        }
    }

     std::string SharedMemorySemaphore::BuildName(const std::string& processName)
    {
        // POSIX shared memory names must start with '/'

        const char* snapName = std::getenv("SNAP_NAME");
        std::string user = SystemInfo::GetUserName();

        if (snapName)
        {
            std::string name = "/snap." + std::string(snapName) + "." + user + "_" + processName + "_sem";
            return name;
        }
        else
        {
            std::string name = "/" + user + "_" + processName + "_sem";
            return name;
        }

        
    }

};
