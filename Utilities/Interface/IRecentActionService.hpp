#ifndef AUTOMATIOTEST_INTERFACE_IRECENTACTIONSERVICE_HPP
#define AUTOMATIOTEST_INTERFACE_IRECENTACTIONSERVICE_HPP

#include "../CommonTypes.hpp"

namespace automationtest::utilities::interface {

class IRecentActionService {
public:
    virtual ~IRecentActionService() = default;

    [[nodiscard]] virtual const automationtest::utilities::Bitmap* RecentMost() const noexcept = 0;
    [[nodiscard]] virtual const automationtest::utilities::Bitmap* RecentMost1() const noexcept = 0;
    [[nodiscard]] virtual const automationtest::utilities::Bitmap* RecentMost2() const noexcept = 0;
    [[nodiscard]] virtual automationtest::utilities::Point LatestClick() const noexcept = 0;

    virtual void Clear() = 0;
    virtual void PushBitmap(const automationtest::utilities::Bitmap& bitmap) = 0;
    virtual void PushLocation(automationtest::utilities::Point point) = 0;
};

} // namespace automationtest::utilities::interface

#endif // AUTOMATIOTEST_INTERFACE_IRECENTACTIONSERVICE_HPP
