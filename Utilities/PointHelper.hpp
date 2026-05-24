#ifndef AUTOMATIOTEST_POINTHELPER_HPP
#define AUTOMATIOTEST_POINTHELPER_HPP

#include "CommonTypes.hpp"
#include "Types/ShortPoint.hpp"

#include <map>
#include <optional>
#include <tuple>
#include <vector>

namespace automationtest::utilities {

class PointHelper {
public:
    static Point Shift(const Point& left, const Point& right) noexcept;
    static bool TwoPointIsFaraway(const Point& p1, const Point& p2, int delta_x = 50, int delta_y = 50) noexcept;
    static bool TwoPointsAreFaraway(const Point& p1, const Point& p2, int delta_x = 50, int delta_y = 50) noexcept;
    static bool TwoPointsAreClose(const Point& p1, const Point& p2, int delta_x = 50, int delta_y = 50) noexcept;
    static std::vector<Point> GeneratePointsFromPoint1ToPoint2(const Point& p1, const Point& p2);
    static Rectangle FindBoundingRectangle(const std::vector<Point>& points) noexcept;
    static std::optional<Point> ToPoint(const std::string& value);
    static std::vector<Point> MergeClosePoints(const std::vector<Point>& points, double delta);
    static std::vector<Point> MergeClosePoints(const std::vector<Point>& points, double delta_x, double delta_y);
    static double Distance(const Point& a, const Point& b) noexcept;
    static double DistanceSquare(const Point& a, const Point& b) noexcept;
    static std::tuple<int, int, int, int> GetMinMax(const Point& p1, const Point& p2) noexcept;
    static std::tuple<short, short, short, short> GetMinMax(const types::ShortPoint& p1, const types::ShortPoint& p2) noexcept;
    static std::tuple<int, int, int, int> GetMinMax(const Point& p1, const Point& p2, const Point& p3) noexcept;
    static std::tuple<short, short, short, short> GetMinMax(const types::ShortPoint& p1, const types::ShortPoint& p2, const types::ShortPoint& p3) noexcept;
    static bool IsCloseToAny(const Point& point, const std::vector<Point>& points, int delta) noexcept;
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_POINTHELPER_HPP
