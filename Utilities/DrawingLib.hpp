#ifndef AUTOMATIOTEST_DRAWINGLIB_HPP
#define AUTOMATIOTEST_DRAWINGLIB_HPP

#include "CommonTypes.hpp"
#include "eOrientation.hpp"

#include <vector>

namespace automationtest::utilities {

class LineWithDescription;
class TriangleWithDescription;

class DrawingLib {
public:
    static int DistanceSquareOfPoint(const Point& p1, const Point& p2) noexcept;
    static double GetDistance(const Point& p1, const Point& p2) noexcept;
    static double GetDistance(double x1, double y1, double x2, double y2) noexcept;
    static Rectangle CreateRectangleFromPoint(const Point& p1, const Point& p2) noexcept;
    static Point FindTopleft(const std::vector<Point>& points) noexcept;
    static Point FindBottomRight(const std::vector<Point>& points) noexcept;
    static Rectangle GetMinRectangle(const std::vector<Point>& points) noexcept;
    static bool Rectangle2IsWordDistanceUpRectangle1(const Rectangle& rect1, const Rectangle& rect2, int left_right_align_space, int line_spacing = 8) noexcept;
    static bool Rectangle2IsWordDistanceLeftRectangle1(const Rectangle& rect1, const Rectangle& rect2, int left_right_align_space, int line_spacing = 8) noexcept;
    static Point GetCenterPointOfRectangle(const Rectangle& rect) noexcept;
    static double PolygonArea(const std::vector<Point>& polygon) noexcept;
    static bool IsTwoPointsHorizonOrVertical(const Point& p1, const Point& p2, eOrientation& orientation) noexcept;
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_DRAWINGLIB_HPP
