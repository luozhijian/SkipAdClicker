#include "TriangleFromLines.hpp"

#include "LineSegment2DHelper.hpp"

#include "../Utilities/MathLib.hpp"
#include "../Utilities/PointHelper.hpp"
#include "../Utilities/Types/ShortLine.hpp"
#include "../Utilities/Types/ShortPoint.hpp"

#include <cmath>

namespace automationtest::opencvlib {

using automationtest::utilities::MathLib;
using automationtest::utilities::PointHelper;
using automationtest::utilities::TriangleWithDescription;
using automationtest::utilities::types::ShortLine;
using automationtest::utilities::types::ShortPoint;

namespace {

bool PointNearSegment(const ShortPoint& point, const ShortLine& segment, short max_gap)
{
    const auto [min_x, max_x, min_y, max_y] = PointHelper::GetMinMax(segment.Point1(), segment.Point2());
    return MathLib::Between(point.x, min_x - max_gap, max_x + max_gap)
        && MathLib::Between(point.y, min_y - max_gap, max_y + max_gap);
}

bool TryIntersectWithGap(const ShortLine& first, const ShortLine& second, short max_gap, ShortPoint& intersection)
{
    const auto [x1, y1, x2, y2] = first.ToShortXY();
    const auto [x3, y3, x4, y4] = second.ToShortXY();

    const auto denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (std::abs(denominator) < 1e-6f) {
        return false;
    }

    const auto pre = (x1 * y2 - y1 * x2);
    const auto post = (x3 * y4 - y3 * x4);
    const auto ix = static_cast<float>(pre * (x3 - x4) - (x1 - x2) * post) / static_cast<float>(denominator);
    const auto iy = static_cast<float>(pre * (y3 - y4) - (y1 - y2) * post) / static_cast<float>(denominator);

    const auto point = ShortPoint(static_cast<int>(std::lround(ix)), static_cast<int>(std::lround(iy)));
    if (!PointNearSegment(point, first, max_gap) || !PointNearSegment(point, second, max_gap)) {
        return false;
    }

    intersection = point;
    return true;
}

std::vector<std::tuple<ShortPoint, ShortPoint, ShortPoint>> DetectInternal(
    const std::vector<ShortLine>& vertical_lines,
    const std::vector<ShortLine>& positive_slope,
    const std::vector<ShortLine>& negative_slope,
    short max_gap,
    short min_size)
{
    std::vector<std::tuple<ShortPoint, ShortPoint, ShortPoint>> triangles {};
    for (const auto& vertical : vertical_lines) {
        for (const auto& positive : positive_slope) {
            if (positive.MaxX() < vertical.x1 + max_gap || positive.MinX() > vertical.x1 + max_gap) {
                continue;
            }
            for (const auto& negative : negative_slope) {
                if (negative.MaxX() < vertical.x1 + max_gap || negative.MinX() > vertical.x1 + max_gap) {
                    continue;
                }

                ShortPoint p23 {};
                ShortPoint p12 {};
                ShortPoint p31 {};
                if (!TryIntersectWithGap(positive, negative, max_gap, p23)) {
                    continue;
                }
                if (p23.x < vertical.x1 + min_size) {
                    continue;
                }
                if (!TryIntersectWithGap(vertical, positive, max_gap, p12) || !TryIntersectWithGap(negative, vertical, max_gap, p31)) {
                    continue;
                }

                const auto area2 = std::abs(
                    p12.x * (p23.y - p31.y) +
                    p23.x * (p31.y - p12.y) +
                    p31.x * (p12.y - p23.y));
                if (area2 <= 0) {
                    continue;
                }

                triangles.emplace_back(p12, p23, p31);
            }
        }
    }
    return triangles;
}

} // namespace

std::vector<TriangleWithDescription> TriangleFromLines::FromLines(const std::vector<LineSegment2D>& lines)
{
    std::vector<ShortLine> vertical_lines {};
    std::vector<ShortLine> positive_slope {};
    std::vector<ShortLine> negative_slope {};

    constexpr short max_gap = 6;
    for (const auto& line : lines) {
        if (line.Length() < 12.0 || line.Length() > 100.0) {
            continue;
        }

        if (std::abs(line.p1.x - line.p2.x) <= max_gap) {
            vertical_lines.push_back(LineSegment2DHelper::ToShortPoint(line));
            continue;
        }

        if (std::abs(line.p1.y - line.p2.y) <= max_gap) {
            continue;
        }

        const auto diff_x = line.p1.x - line.p2.x;
        const auto diff_y = line.p1.y - line.p2.y;
        if ((diff_x > 0) != (diff_y > 0)) {
            negative_slope.push_back(LineSegment2DHelper::ToShortPoint(line));
        } else {
            positive_slope.push_back(LineSegment2DHelper::ToShortPoint(line));
        }
    }

    std::vector<TriangleWithDescription> result {};
    for (const auto& [a, b, c] : DetectInternal(vertical_lines, positive_slope, negative_slope, max_gap, 10)) {
        result.emplace_back(a.ToPoint(), b.ToPoint(), c.ToPoint());
    }

    return result;
}

} // namespace automationtest::opencvlib
