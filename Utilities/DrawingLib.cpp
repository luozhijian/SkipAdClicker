#include "DrawingLib.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace automationtest::utilities {

int DrawingLib::DistanceSquareOfPoint(const Point& p1, const Point& p2) noexcept
{
    return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

double DrawingLib::GetDistance(const Point& p1, const Point& p2) noexcept
{
    return GetDistance(static_cast<double>(p1.x), static_cast<double>(p1.y), static_cast<double>(p2.x), static_cast<double>(p2.y));
}

double DrawingLib::GetDistance(double x1, double y1, double x2, double y2) noexcept
{
    return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

Rectangle DrawingLib::CreateRectangleFromPoint(const Point& p1, const Point& p2) noexcept
{
    const int top = std::min(p1.y, p2.y);
    const int bottom = std::max(p1.y, p2.y);
    const int left = std::min(p1.x, p2.x);
    const int right = std::max(p1.x, p2.x);
    return Rectangle {left, top, right - left, bottom - top};
}

Point DrawingLib::FindTopleft(const std::vector<Point>& points) noexcept
{
    int x = std::numeric_limits<int>::max();
    int y = std::numeric_limits<int>::max();
    for (const auto& point : points) {
        x = std::min(x, point.x);
        y = std::min(y, point.y);
    }
    return Point {x, y};
}

Point DrawingLib::FindBottomRight(const std::vector<Point>& points) noexcept
{
    int x = std::numeric_limits<int>::min();
    int y = std::numeric_limits<int>::min();
    for (const auto& point : points) {
        x = std::max(x, point.x);
        y = std::max(y, point.y);
    }
    return Point {x, y};
}

Rectangle DrawingLib::GetMinRectangle(const std::vector<Point>& points) noexcept
{
    const auto top_left = FindTopleft(points);
    const auto bottom_right = FindBottomRight(points);
    return Rectangle {top_left.x, top_left.y, bottom_right.x - top_left.x, bottom_right.y - top_left.y};
}

bool DrawingLib::Rectangle2IsWordDistanceUpRectangle1(const Rectangle& rect1, const Rectangle& rect2, int left_right_align_space, int line_spacing) noexcept
{
    const int difference_y = rect1.y - rect2.y - rect2.height;
    return std::abs(rect2.x - rect1.x) < left_right_align_space && difference_y < line_spacing && difference_y >= -2;
}

bool DrawingLib::Rectangle2IsWordDistanceLeftRectangle1(const Rectangle& rect1, const Rectangle& rect2, int left_right_align_space, int line_spacing) noexcept
{
    const int difference_x = rect1.x - rect2.x - rect2.width;
    return std::abs(rect2.y - rect1.y) < line_spacing && difference_x < left_right_align_space && difference_x >= -2;
}

Point DrawingLib::GetCenterPointOfRectangle(const Rectangle& rect) noexcept
{
    return Point {rect.x + rect.width / 2, rect.y + rect.height / 2};
}

double DrawingLib::PolygonArea(const std::vector<Point>& polygon) noexcept
{
    double area = 0.0;
    for (std::size_t i = 0; i < polygon.size(); ++i) {
        const std::size_t j = (i + 1) % polygon.size();
        area += polygon[i].x * polygon[j].y;
        area -= polygon[i].y * polygon[j].x;
    }
    area /= 2.0;
    return area < 0.0 ? -area : area;
}

bool DrawingLib::IsTwoPointsHorizonOrVertical(const Point& p1, const Point& p2, eOrientation& orientation) noexcept
{
    orientation = eOrientation::Horizontal;
    if (std::abs(p1.x - p2.x) <= 1) {
        orientation = eOrientation::Vertical;
        return true;
    }
    if (std::abs(p1.y - p2.y) <= 1) {
        orientation = eOrientation::Horizontal;
        return true;
    }
    return false;
}

} // namespace automationtest::utilities
