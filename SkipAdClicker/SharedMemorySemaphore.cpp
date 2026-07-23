// SharedMemorySemaphore.hpp

#include <chrono>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#ifndef _WIN32
#include <fcntl.h>
#endif
#include <iostream>
#include <stdexcept>
#ifndef _WIN32
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#include <thread>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#undef GetUserName
#endif

#include  "SharedMemorySemaphore.hpp"
#include  "MainWindow.hpp"
#include  "../Utilities/Logger.hpp"
#include  "../Utilities/OSRelated/SystemInfo.hpp"
#include <QCoreApplication>
#include <QMetaObject>
namespace automationtest::app {

namespace {
    constexpr std::uint32_t kInitializedMagic = 0x534d5348; // "SMSH"
    constexpr std::uint32_t kCommandShow = 1;
    constexpr std::uint32_t kCommandQuit = 2;
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
#ifdef _WIN32
        HANDLE semaphore = ::OpenSemaphoreA(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, FALSE, name_.c_str());
        if (semaphore == nullptr)
            return false;

        ::CloseHandle(semaphore);
        return true;
#else
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
#endif
    }

    // Clear / remove shared memory semaphore
    void SharedMemorySemaphore::Clear()
    {
        StopThreadToWait();

#ifdef _WIN32
        Close();
#else
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
#endif
    }

    // Creator creates semaphore and ws for consumer trigger
    bool SharedMemorySemaphore::CreateForCreator(const std::string& version, unsigned int initialValue)
    {
        std::cout <<  "CreateForCreator";

        Close();
#ifdef _WIN32
        semaphore_ = ::CreateSemaphoreA(
            nullptr,
            static_cast<LONG>(initialValue),
            LONG_MAX,
            name_.c_str());
        if (semaphore_ == nullptr)
        {
            automationtest::utilities::Logger::Error(
                "CreateSemaphore failed in CreateForCreator(): " + name_);
            return false;
        }
        if (::GetLastError() == ERROR_ALREADY_EXISTS)
        {
            Close();
            return false;
        }

        const auto stateName = BuildStateName(name_);
        shared_memory_ = ::CreateFileMappingA(
            INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0,
            sizeof(SharedSemaphoreState), stateName.c_str());
        if (shared_memory_ == nullptr)
        {
            automationtest::utilities::Logger::Error(
                "CreateFileMapping failed in CreateForCreator(): " + stateName);
            Close();
            return false;
        }
        state_ = static_cast<SharedSemaphoreState*>(
            ::MapViewOfFile(shared_memory_, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedSemaphoreState)));
        if (state_ == nullptr)
        {
            automationtest::utilities::Logger::Error(
                "MapViewOfFile failed in CreateForCreator(): " + stateName);
            Close();
            return false;
        }
        std::memset(state_, 0, sizeof(SharedSemaphoreState));
        std::strncpy(state_->version, version.c_str(), sizeof(state_->version) - 1);
        state_->initialized = kInitializedMagic;
#else
        shm_fd_ = shm_open(
            name_.c_str(),
            O_CREAT | O_EXCL | O_RDWR,
            0666);

        if (shm_fd_ == -1)
        {
            automationtest::utilities::Logger::Error(
                "shm_open failed in CreateForCreator(): " + name_ + " : " + std::string(std::strerror(errno)));
            std::cout <<    
                "shm_open failed in CreateForCreator(): " + std::string(std::strerror(errno));
            return false;
        }

        if (ftruncate(shm_fd_, sizeof(SharedSemaphoreState)) == -1)
        {
            automationtest::utilities::Logger::Error(
                "ftruncate failed in CreateForCreator(): " + name_ + " : " + std::string(std::strerror(errno)));
            Close();
            Clear();
            return false;
        }

        if (!MapSharedMemory(shm_fd_))
        {
            Close();
            Clear();
            return false;
        }

        std::memset(state_, 0, sizeof(SharedSemaphoreState));
        if (sem_init(&state_->semaphore, 1, initialValue) == -1)
        {
            automationtest::utilities::Logger::Error(
                "sem_init failed in CreateForCreator(): " + name_ + " : " + std::string(std::strerror(errno)));
            Close();
            Clear();
            return false;
        }

        std::strncpy(state_->version, version.c_str(), sizeof(state_->version) - 1);
        state_->initialized = kInitializedMagic;
#endif
        return true;
    }

    // Consumer opens existing semaphore
    void SharedMemorySemaphore::OpenForConsumer()
    {
        Close();

#ifdef _WIN32
        semaphore_ = ::OpenSemaphoreA(
            SYNCHRONIZE | SEMAPHORE_MODIFY_STATE,
            FALSE,
            name_.c_str());
        if (semaphore_ == nullptr)
        {
            automationtest::utilities::Logger::Error(
                "OpenSemaphore failed in OpenForConsumer(): " + name_);
            return;
        }
        const auto stateName = BuildStateName(name_);
        shared_memory_ = ::OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, stateName.c_str());
        if (shared_memory_ != nullptr)
        {
            state_ = static_cast<SharedSemaphoreState*>(
                ::MapViewOfFile(shared_memory_, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedSemaphoreState)));
        }
#else
        shm_fd_ = shm_open(name_.c_str(), O_RDWR, 0);
        if (shm_fd_ == -1)
        {
            automationtest::utilities::Logger::Error(
                "shm_open failed in OpenForConsumer(): " + name_ + " : " + std::string(std::strerror(errno)));
            return;
        }

        struct stat sharedMemoryInfo {};
        if (fstat(shm_fd_, &sharedMemoryInfo) == -1
            || sharedMemoryInfo.st_size < static_cast<off_t>(sizeof(SharedSemaphoreState))
            || !MapSharedMemory(shm_fd_))
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
#endif
    }

    std::string SharedMemorySemaphore::RunningVersion() const
    {
        if (state_ == nullptr || state_->initialized != kInitializedMagic)
            return {};
        return std::string(state_->version, strnlen(state_->version, sizeof(state_->version)));
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

#ifndef _WIN32
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
#endif

    bool SharedMemorySemaphore::WaitForSignal(int timeoutMs)
    {
#ifdef _WIN32
        return semaphore_ != nullptr
            && ::WaitForSingleObject(semaphore_, static_cast<DWORD>(timeoutMs)) == WAIT_OBJECT_0;
#else
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
#endif
    }

    void SharedMemorySemaphore::RealWait(std::stop_token stoken)
    {
#ifdef _WIN32
        if (semaphore_ == nullptr)
#else
        if (state_ == nullptr)
#endif
        {
            automationtest::utilities::Logger::Error ("Shared memory semaphore is not opened.");
            return;
        }

        while (!stoken.stop_requested()) 
        {
#ifdef _WIN32
            if (semaphore_ == nullptr)
#else
            if (state_ == nullptr ) 
#endif
                break;
            if ( WaitForSignal ( 200)  )
            {
                const auto command = state_ == nullptr ? kCommandShow : state_->command;
                if (command == kCommandQuit)
                {
                    QMetaObject::invokeMethod(
                        QCoreApplication::instance(), &QCoreApplication::quit, Qt::QueuedConnection);
                    break;
                }
                this->mainWin->ShowAndRestore();
            }
        }   

    }

    // Consumer triggers creator
    void SharedMemorySemaphore::Trigger()
    {
        SetCommand(kCommandShow);
        Signal();
    }

    void SharedMemorySemaphore::Signal()
    {
#ifdef _WIN32
        if (semaphore_ == nullptr)
            throw std::runtime_error("Shared memory semaphore is not opened.");

        if (!::ReleaseSemaphore(semaphore_, 1, nullptr))
            throw std::runtime_error("ReleaseSemaphore failed.");
#else
        if (state_ == nullptr)
            throw std::runtime_error("Shared memory semaphore is not opened.");

        if (sem_post(&state_->semaphore) == -1)
        {
            throw std::runtime_error(
                "sem_post failed: " + std::string(std::strerror(errno)));
        }
#endif
    }

    void SharedMemorySemaphore::SetCommand(std::uint32_t command)
    {
        if (state_ != nullptr)
            state_->command = command;
    }

    void SharedMemorySemaphore::RequestQuit()
    {
        SetCommand(kCommandQuit);
        Signal();
    }

    void SharedMemorySemaphore::Close()
    {
        StopThreadToWait();

#ifdef _WIN32
        if (semaphore_ != nullptr)
        {
            ::CloseHandle(semaphore_);
            semaphore_ = nullptr;
        }
        if (state_ != nullptr)
        {
            ::UnmapViewOfFile(state_);
            state_ = nullptr;
        }
        if (shared_memory_ != nullptr)
        {
            ::CloseHandle(shared_memory_);
            shared_memory_ = nullptr;
        }
#else
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
#endif
    }

     std::string SharedMemorySemaphore::BuildName(const std::string& processName)
    {
        const char* snapName = std::getenv("SNAP_NAME");
        std::string user = (SystemInfo::GetUserName)();

#ifdef _WIN32
        std::string name = "Local\\" + user + "_" + processName + "_sem";
        for (char& character : name)
        {
            if (character == '\\' || character == '/')
                character = '_';
        }
        name.replace(0, 6, "Local\\");
        return name;
#else
        // POSIX shared memory names must start with '/'
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
#endif

        
    }

    std::string SharedMemorySemaphore::BuildStateName(const std::string& semaphoreName)
    {
        return semaphoreName + "_state";
    }

};
