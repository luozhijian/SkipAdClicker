#include "TriangleWithDescription.hpp"

#include "DrawingLib.hpp"
#include "MathLib.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>

namespace automationtest::utilities {

TriangleWithDescription::TriangleWithDescription(Point p1, Point p2, Point p3)
    : point1(p1)
    , point2(p2)
    , point3(p3)
{
}

TriangleWithDescription TriangleWithDescription::Expand(int expand_pixels) const
{
    if (expand_pixels == 0) {
        return *this;
    }

    struct DoublePoint {
        double x;
        double y;
    };

    const DoublePoint points[] {
        {static_cast<double>(point1.x), static_cast<double>(point1.y)},
        {static_cast<double>(point2.x), static_cast<double>(point2.y)},
        {static_cast<double>(point3.x), static_cast<double>(point3.y)}
    };
    const double signed_double_area =
        points[0].x * (points[1].y - points[2].y)
        + points[1].x * (points[2].y - points[0].y)
        + points[2].x * (points[0].y - points[1].y);

    if (signed_double_area == 0.0) {
        return *this;
    }

    DoublePoint shifted_starts[3] {};
    DoublePoint directions[3] {};
    const double orientation = signed_double_area > 0.0 ? 1.0 : -1.0;

    for (int i = 0; i < 3; ++i) {
        const DoublePoint start = points[i];
        const DoublePoint end = points[(i + 1) % 3];
        const DoublePoint direction {end.x - start.x, end.y - start.y};
        const double length = std::hypot(direction.x, direction.y);
        if (length == 0.0) {
            return *this;
        }

        const double offset = static_cast<double>(expand_pixels) * orientation;
        shifted_starts[i] = {
            start.x + offset * direction.y / length,
            start.y - offset * direction.x / length
        };
        directions[i] = direction;
    }

    const auto intersect = [&](int first, int second) {
        const DoublePoint delta {
            shifted_starts[second].x - shifted_starts[first].x,
            shifted_starts[second].y - shifted_starts[first].y
        };
        const double cross =
            directions[first].x * directions[second].y
            - directions[first].y * directions[second].x;
        const double distance_along_first =
            (delta.x * directions[second].y - delta.y * directions[second].x) / cross;
        return Point {
            static_cast<int>(std::lround(shifted_starts[first].x + distance_along_first * directions[first].x)),
            static_cast<int>(std::lround(shifted_starts[first].y + distance_along_first * directions[first].y))
        };
    };

    return TriangleWithDescription(intersect(2, 0), intersect(0, 1), intersect(1, 2));
}

std::vector<Point> TriangleWithDescription::AsArray() const
{
    return {point1, point2, point3};
}

Rectangle TriangleWithDescription::GetCoveredRectangle() const
{
    return DrawingLib::GetMinRectangle(AsArray());
}

bool TriangleWithDescription::IsEachEdgeTriangleLargeThan(int square_of_length) const
{
    return DrawingLib::DistanceSquareOfPoint(point1, point2) >= square_of_length
        && DrawingLib::DistanceSquareOfPoint(point2, point3) >= square_of_length
        && DrawingLib::DistanceSquareOfPoint(point3, point1) >= square_of_length;
}

double TriangleWithDescription::Area() const
{
    return DrawingLib::PolygonArea(AsArray());
}

TriangleWithDescription TriangleWithDescription::CreateAShift(const Size& shift) const
{
    return TriangleWithDescription(
        Point {point1.x + shift.width, point1.y + shift.height},
        Point {point2.x + shift.width, point2.y + shift.height},
        Point {point3.x + shift.width, point3.y + shift.height});
}

Point TriangleWithDescription::Center() const noexcept
{
    return Point {(point1.x + point2.x + point3.x) / 3, (point1.y + point2.y + point3.y) / 3};
}

bool TriangleWithDescription::GetVerticalOrHorizontalLine(int& which_line, eOrientation& orientation) const noexcept
{
    which_line = 0;
    if (DrawingLib::IsTwoPointsHorizonOrVertical(point1, point2, orientation)) {
        return true;
    }
    which_line = 1;
    if (DrawingLib::IsTwoPointsHorizonOrVertical(point2, point3, orientation)) {
        return true;
    }
    which_line = 2;
    return DrawingLib::IsTwoPointsHorizonOrVertical(point3, point1, orientation);
}

bool TriangleWithDescription::IsScrollbarCandidate()
{
    scroll_bar_x_y_value = 0;
    scroll_bar_x_y_center = 0;
    scroll_bar_x_y_1 = 0;
    scroll_bar_x_y_2 = 0;

    if (!GetVerticalOrHorizontalLine(scroll_bar_which_line, scroll_bar_orientation)) {
        return false;
    }

    const auto points = AsArray();
    const Point p1 = points[scroll_bar_which_line];
    const Point p2 = points[(scroll_bar_which_line + 1) % 3];
    const Point p3 = points[(scroll_bar_which_line + 2) % 3];
    const auto distance1 = DrawingLib::GetDistance(p1, p3);
    const auto distance2 = DrawingLib::GetDistance(p2, p3);

    if (scroll_bar_orientation == eOrientation::Horizontal) {
        scroll_bar_x_y_value = p1.y;
        scroll_bar_x_y_center = Center().y;
        scroll_bar_x_y_1 = std::min(p1.x, p2.x);
        scroll_bar_x_y_2 = std::max(p1.x, p2.x);
    } else {
        scroll_bar_x_y_value = p1.x;
        scroll_bar_x_y_center = Center().x;
        scroll_bar_x_y_1 = std::min(p1.y, p2.y);
        scroll_bar_x_y_2 = std::max(p1.y, p2.y);
    }

    return std::abs(distance1 - distance2) <= 2.0;
}

void TriangleWithDescription::Shift(const Size& size) noexcept
{
    point1 = Point {point1.x + size.width, point1.y + size.height};
    point2 = Point {point2.x + size.width, point2.y + size.height};
    point3 = Point {point3.x + size.width, point3.y + size.height};
}

void TriangleWithDescription::CalculateAngles()
{
    angle_a = AngleBetween(point1, point2, point3);
    angle_b = AngleBetween(point1, point3, point2);
    angle_c = AngleBetween(point2, point1, point3);
}

bool TriangleWithDescription::IsScrollBarOrDropDownArrow()
{
    CalculateAngles();
    return MathLib::MinValue(std::vector<double> {angle_a, angle_b, angle_c}) >= 15.0;
}

std::string TriangleWithDescription::ToString() const
{
    std::ostringstream builder;
    builder << "Triangle " << point1.ToString() << ' ' << point2.ToString() << ' ' << point3.ToString();
    return builder.str();
}

double TriangleWithDescription::AngleBetween(const Point& p1, const Point& vertex, const Point& p2)
{
    const double v1x = p1.x - vertex.x;
    const double v1y = p1.y - vertex.y;
    const double v2x = p2.x - vertex.x;
    const double v2y = p2.y - vertex.y;
    const double dot = v1x * v2x + v1y * v2y;
    const double mag1 = std::sqrt(v1x * v1x + v1y * v1y);
    const double mag2 = std::sqrt(v2x * v2x + v2y * v2y);
    if (mag1 == 0.0 || mag2 == 0.0) {
        return 0.0;
    }
    double cos_theta = dot / (mag1 * mag2);
    cos_theta = std::max(-1.0, std::min(1.0, cos_theta));
    return std::acos(cos_theta) * 180.0 / 3.14159265358979323846;
}

} // namespace automationtest::utilities
