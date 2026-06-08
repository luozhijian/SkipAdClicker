#ifndef AUTOMATIOTEST_MANUALRESETEVENT_HPP
#define AUTOMATIOTEST_MANUALRESETEVENT_HPP

#include <condition_variable>
#include <mutex>

namespace automationtest::utilities {

class ManualResetEvent {
public:
    explicit ManualResetEvent(bool is_set = false) noexcept;
    ManualResetEvent(const ManualResetEvent&) = delete;
    ManualResetEvent& operator=(const ManualResetEvent&) = delete;

    void Set();
    void Reset();
    void WaitWhileSet() const;
    [[nodiscard]] bool IsSet() const;

private:
    mutable std::mutex mutex_ {};
    mutable std::condition_variable condition_ {};
    bool is_set_ {false};
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_MANUALRESETEVENT_HPP
