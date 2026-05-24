#ifndef AUTOMATIOTEST_POINTWITHANGLEANDDISTANCE_HPP
#define AUTOMATIOTEST_POINTWITHANGLEANDDISTANCE_HPP

#include "CommonTypes.hpp"

namespace automationtest::utilities {

class PointWithAngleAndDistance {
public:
    explicit PointWithAngleAndDistance(Point point_value);
    PointWithAngleAndDistance(int x, int y);

    [[nodiscard]] const Point& Value() const noexcept;
    [[nodiscard]] double RadiusSquared() const noexcept;
    [[nodiscard]] double Theta() const noexcept;

    friend bool operator==(const PointWithAngleAndDistance&, const PointWithAngleAndDistance&) = default;

private:
    Point point_ {};
    double radius_squared_ {0.0};
    double theta_ {0.0};
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_POINTWITHANGLEANDDISTANCE_HPP
