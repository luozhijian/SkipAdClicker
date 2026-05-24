#include "PointHelper.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <regex>

namespace automationtest::utilities {

Point PointHelper::Shift(const Point& left, const Point& right) noexcept
{
    return Point {left.x + right.x, left.y + right.y};
}

bool PointHelper::TwoPointIsFaraway(const Point& p1, const Point& p2, int delta_x, int delta_y) noexcept
{
    return TwoPointsAreFaraway(p1, p2, delta_x, delta_y);
}

bool PointHelper::TwoPointsAreFaraway(const Point& p1, const Point& p2, int delta_x, int delta_y) noexcept
{
    return std::abs(p1.x - p2.x) > delta_x || std::abs(p1.y - p2.y) > delta_y;
}

bool PointHelper::TwoPointsAreClose(const Point& p1, const Point& p2, int delta_x, int delta_y) noexcept
{
    return !TwoPointsAreFaraway(p1, p2, delta_x, delta_y);
}

std::vector<Point> PointHelper::GeneratePointsFromPoint1ToPoint2(const Point& p1, const Point& p2)
{
    std::vector<Point> points;
    double step_x = 1.0;
    double step_y = 1.0;
    const int delta_y = p2.y - p1.y;
    const int delta_x = p2.x - p1.x;

    int count = 0;
    if (std::abs(delta_x) > std::abs(delta_y)) {
        count = std::abs(delta_x);
        step_x = delta_x > 0 ? 1.0 : -1.0;
        step_y = delta_x == 0 ? 0.0 : delta_y * step_x / static_cast<double>(delta_x);
    } else {
        count = std::abs(delta_y);
        step_y = delta_y > 0 ? 1.0 : -1.0;
        step_x = delta_y == 0 ? 0.0 : delta_x * step_y / static_cast<double>(delta_y);
    }

    for (int i = 1; i < count; ++i) {
        points.push_back(Point {p1.x + static_cast<int>(i * step_x), p1.y + static_cast<int>(i * step_y)});
    }
    points.push_back(p2);
    return points;
}

Rectangle PointHelper::FindBoundingRectangle(const std::vector<Point>& points) noexcept
{
    int min_x = std::numeric_limits<int>::max();
    int min_y = std::numeric_limits<int>::max();
    int max_x = std::numeric_limits<int>::min();
    int max_y = std::numeric_limits<int>::min();

    for (const auto& point : points) {
        min_x = std::min(min_x, point.x);
        max_x = std::max(max_x, point.x);
        min_y = std::min(min_y, point.y);
        max_y = std::max(max_y, point.y);
    }

    return Rectangle {min_x, min_y, max_x - min_x, max_y - min_y};
}

std::optional<Point> PointHelper::ToPoint(const std::string& value)
{
    if (value.empty()) {
        return std::nullopt;
    }

    static const std::regex pattern(R"(Point\s*\(\s*(\d+)\s*,\s*(\d+)\s*\))", std::regex_constants::icase);
    std::smatch match;
    if (!std::regex_match(value, match, pattern)) {
        throw std::runtime_error("Unknown Point: " + value);
    }

    return Point {std::stoi(match[1].str()), std::stoi(match[2].str())};
}

std::vector<Point> PointHelper::MergeClosePoints(const std::vector<Point>& points, double delta)
{
    const double distance_square = delta * delta;
    std::vector<Point> merged_points;
    std::vector<bool> visited(points.size(), false);

    for (std::size_t i = 0; i < points.size(); ++i) {
        if (visited[i]) {
            continue;
        }

        std::vector<Point> cluster {points[i]};
        visited[i] = true;
        for (std::size_t j = i + 1; j < points.size(); ++j) {
            if (visited[j]) {
                continue;
            }
            if (DistanceSquare(points[i], points[j]) < distance_square) {
                cluster.push_back(points[j]);
                visited[j] = true;
            }
        }

        int sum_x = 0;
        int sum_y = 0;
        for (const auto& point : cluster) {
            sum_x += point.x;
            sum_y += point.y;
        }
        merged_points.push_back(Point {
            static_cast<int>(std::round(sum_x / static_cast<double>(cluster.size()))),
            static_cast<int>(std::round(sum_y / static_cast<double>(cluster.size())))
        });
    }

    return merged_points;
}

std::vector<Point> PointHelper::MergeClosePoints(const std::vector<Point>& points, double delta_x, double delta_y)
{
    std::vector<Point> merged_points;
    std::vector<bool> visited(points.size(), false);

    for (std::size_t i = 0; i < points.size(); ++i) {
        if (visited[i]) {
            continue;
        }

        std::vector<Point> cluster {points[i]};
        visited[i] = true;
        for (std::size_t j = i + 1; j < points.size(); ++j) {
            if (visited[j]) {
                continue;
            }
            if (std::abs(points[i].x - points[j].x) <= delta_x && std::abs(points[i].y - points[j].y) <= delta_y) {
                cluster.push_back(points[j]);
                visited[j] = true;
            }
        }

        int sum_x = 0;
        int sum_y = 0;
        for (const auto& point : cluster) {
            sum_x += point.x;
            sum_y += point.y;
        }
        merged_points.push_back(Point {
            static_cast<int>(std::round(sum_x / static_cast<double>(cluster.size()))),
            static_cast<int>(std::round(sum_y / static_cast<double>(cluster.size())))
        });
    }

    return merged_points;
}

double PointHelper::Distance(const Point& a, const Point& b) noexcept
{
    return std::sqrt(DistanceSquare(a, b));
}

double PointHelper::DistanceSquare(const Point& a, const Point& b) noexcept
{
    const int dx = a.x - b.x;
    const int dy = a.y - b.y;
    return dx * dx + dy * dy;
}

std::tuple<int, int, int, int> PointHelper::GetMinMax(const Point& p1, const Point& p2) noexcept
{
    return {std::min(p1.x, p2.x), std::max(p1.x, p2.x), std::min(p1.y, p2.y), std::max(p1.y, p2.y)};
}

std::tuple<short, short, short, short> PointHelper::GetMinMax(const types::ShortPoint& p1, const types::ShortPoint& p2) noexcept
{
    return {std::min(p1.x, p2.x), std::max(p1.x, p2.x), std::min(p1.y, p2.y), std::max(p1.y, p2.y)};
}

std::tuple<int, int, int, int> PointHelper::GetMinMax(const Point& p1, const Point& p2, const Point& p3) noexcept
{
    return {
        std::min(std::min(p1.x, p2.x), p3.x),
        std::max(std::max(p1.x, p2.x), p3.x),
        std::min(std::min(p1.y, p2.y), p3.y),
        std::max(std::max(p1.y, p2.y), p3.y)
    };
}

std::tuple<short, short, short, short> PointHelper::GetMinMax(const types::ShortPoint& p1, const types::ShortPoint& p2, const types::ShortPoint& p3) noexcept
{
    return {
        std::min(std::min(p1.x, p2.x), p3.x),
        std::max(std::max(p1.x, p2.x), p3.x),
        std::min(std::min(p1.y, p2.y), p3.y),
        std::max(std::max(p1.y, p2.y), p3.y)
    };
}

bool PointHelper::IsCloseToAny(const Point& point, const std::vector<Point>& points, int delta) noexcept
{
    return std::any_of(points.begin(), points.end(), [&](const Point& existing) {
        return TwoPointsAreClose(point, existing, delta, delta);
    });
}

} // namespace automationtest::utilities
