#include "VectorShape.hpp"

#include "../Utilities/RectangleHelper.hpp"

namespace automationtest::opencvlib {

using automationtest::utilities::LineWithDescription;
using automationtest::utilities::Point;
using automationtest::utilities::Rectangle;
using automationtest::utilities::RectangleHelper;

Rectangle VectorShape::IsHorizonVerticalRectangle(const std::vector<Point>& points)
{
    const auto lines = GroupVectorIntoLine(points, 20);
    if (lines.size() != 4 && lines.size() != 6) {
        return {};
    }

    std::vector<LineWithDescription*> line_ptrs {};
    line_ptrs.reserve(lines.size());
    for (const auto& line : lines) {
        line_ptrs.push_back(const_cast<LineWithDescription*>(&line));
    }
    return RectangleHelper::CreateRectangleFromLines(line_ptrs);
}

std::vector<LineWithDescription> VectorShape::GroupVectorIntoLine(const std::vector<Point>& points, int max_allowance)
{
    std::vector<LineWithDescription> result {};
    if (points.empty()) {
        return result;
    }

    auto previous_point = points.back();
    LineWithDescription current {};
    bool has_current = false;

    for (const auto& point : points) {
        if (!has_current) {
            current = LineWithDescription(previous_point, point);
            has_current = true;
        } else if (current.IsExtend(point, max_allowance)) {
            current.Extend(point);
        } else {
            result.push_back(current);
            current = LineWithDescription(previous_point, point);
        }
        previous_point = point;
    }

    if (has_current) {
        result.push_back(current);
    }
    return result;
}

} // namespace automationtest::opencvlib
