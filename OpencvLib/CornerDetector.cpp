#include "CornerDetector.hpp"

#include "../Utilities/PointHelper.hpp"

#include <cmath>

namespace automationtest::opencvlib {

using automationtest::utilities::Point;
using automationtest::utilities::PointHelper;

std::vector<Point> CornerDetector::Detect(const std::vector<Point>& points) const
{
    if (points.empty()) {
        return {};
    }

    auto min_x_top = points.front();
    auto min_x_bottom = points.front();
    auto max_x_top = points.front();
    auto max_x_bottom = points.front();
    auto min_y_left = points.front();
    auto min_y_right = points.front();
    auto max_y_left = points.front();
    auto max_y_right = points.front();

    int max_x = points.front().x;
    int min_x = points.front().x;
    int max_y = points.front().y;
    int min_y = points.front().y;

    for (const auto& point : points) {
        if (point.x > max_x) {
            max_x = point.x;
            max_x_top = point;
            max_x_bottom = point;
        }
        if (point.x < min_x) {
            min_x = point.x;
            min_x_top = point;
            min_x_bottom = point;
        }
        if (point.y > max_y) {
            max_y = point.y;
            max_y_left = point;
            max_y_right = point;
        }
        if (point.y < min_y) {
            min_y = point.y;
            min_y_left = point;
            min_y_right = point;
        }

        if (point.x == min_x) {
            if (point.y < min_x_top.y) {
                min_x_top = point;
            }
            if (point.y > min_x_bottom.y) {
                min_x_bottom = point;
            }
        }
        if (point.x == max_x) {
            if (point.y < max_x_top.y) {
                max_x_top = point;
            }
            if (point.y > max_x_bottom.y) {
                max_x_bottom = point;
            }
        }
        if (point.y == min_y) {
            if (point.x < min_y_left.x) {
                min_y_left = point;
            }
            if (point.x > min_y_right.x) {
                min_y_right = point;
            }
        }
        if (point.y == max_y) {
            if (point.x < max_y_left.x) {
                max_y_left = point;
            }
            if (point.x > max_y_right.x) {
                max_y_right = point;
            }
        }
    }

    const std::vector<Point> corners {
        min_y_left, min_y_right, max_x_top, max_x_bottom,
        max_y_right, max_y_left, min_x_bottom, min_x_top
    };

    const auto delta_x = std::abs(min_x - max_x) / 4.0;
    const auto delta_y = std::abs(min_y - max_y) / 4.0;
    return PointHelper::MergeClosePoints(corners, delta_x, delta_y);
}

} // namespace automationtest::opencvlib
