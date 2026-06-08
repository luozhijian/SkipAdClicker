#include "ManualResetEvent.hpp"

namespace automationtest::utilities {

ManualResetEvent::ManualResetEvent(bool is_set) noexcept
    : is_set_(is_set)
{
}

void ManualResetEvent::Set()
{
    {
        std::lock_guard lock(mutex_);
        is_set_ = true;
    }
    condition_.notify_all();
}

void ManualResetEvent::Reset()
{
    {
        std::lock_guard lock(mutex_);
        is_set_ = false;
    }
    condition_.notify_all();
}

void ManualResetEvent::WaitWhileSet() const
{
    std::unique_lock lock(mutex_);
    condition_.wait(lock, [this]() {
        return !is_set_;
    });
}

bool ManualResetEvent::IsSet() const
{
    std::lock_guard lock(mutex_);
    return is_set_;
}

} // namespace automationtest::utilities
