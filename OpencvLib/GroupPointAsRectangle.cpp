#include "GroupPointAsRectangle.hpp"

#include <algorithm>

namespace automationtest::opencvlib {

using automationtest::utilities::Rectangle;

void GroupPointAsRectangle::AddPoint(int x, int y)
{
    bool processed = false;
    std::vector<std::size_t> expanded {};

    for (std::size_t index = 0; index < rectangles_corners_.size(); ++index) {
        auto& group = rectangles_corners_[index];
        if (PointInRange(x, y, group)) {
            ++group[4];
            processed = true;
        } else if (PointNearRange(x, y, group, max_distance_)) {
            ExpandRange(x, y, group);
            expanded.push_back(index);
            processed = true;
        }
    }

    if (!processed) {
        rectangles_corners_.push_back(AddOneGroup(x, y));
        return;
    }

    if (expanded.size() > 1) {
        auto first = rectangles_corners_[expanded.front()];
        for (std::size_t offset = expanded.size(); offset-- > 1;) {
            const auto current_index = expanded[offset];
            MergeTwoRangeIntoOne(first, rectangles_corners_[current_index]);
            rectangles_corners_.erase(rectangles_corners_.begin() + static_cast<std::ptrdiff_t>(current_index));
        }
        rectangles_corners_[expanded.front()] = first;
    }
}

void GroupPointAsRectangle::CompletedAdding()
{
    CheckAndMergeRangeIfItIsNear();
}

std::vector<Rectangle> GroupPointAsRectangle::GetGroupedResult() const
{
    std::vector<Rectangle> result {};
    for (const auto& range : rectangles_corners_) {
        const auto width = range[2] - range[0] + 1;
        const auto height = range[3] - range[1] + 1;
        if (width <= 0 || height <= 0) {
            continue;
        }
        result.push_back(Rectangle {range[0], range[1], width, height});
    }
    return result;
}

bool GroupPointAsRectangle::PointInRange(int x, int y, const RectangleGroup& ranges) noexcept
{
    return ranges[0] <= x && ranges[2] >= x && ranges[1] <= y && ranges[3] >= y;
}

bool GroupPointAsRectangle::PointNearRange(int x, int y, const RectangleGroup& ranges, int distance) const noexcept
{
    const auto x1 = ranges[0];
    const auto y1 = ranges[1];
    const auto x2 = ranges[2];
    const auto y2 = ranges[3];

    const auto squared_distance = [distance](int dx, int dy) {
        return dx * dx + dy * dy <= distance;
    };

    if (x1 <= x && x2 >= x) {
        if (y1 <= y && y2 >= y) {
            return true;
        }
        const auto diff = std::min(std::abs(y1 - y), std::abs(y2 - y));
        return diff <= max_distance_ && diff * diff <= distance;
    }

    if (y1 <= y && y2 >= y) {
        const auto diff = std::min(std::abs(x1 - x), std::abs(x2 - x));
        return diff <= max_distance_ && diff * diff <= distance;
    }

    const auto nearest_x = x < x1 ? x1 : x2;
    const auto nearest_y = y < y1 ? y1 : y2;
    return squared_distance(nearest_x - x, nearest_y - y);
}

void GroupPointAsRectangle::ExpandRange(int x, int y, RectangleGroup& ranges) noexcept
{
    ranges[0] = std::min(ranges[0], x);
    ranges[1] = std::min(ranges[1], y);
    ranges[2] = std::max(ranges[2], x);
    ranges[3] = std::max(ranges[3], y);
    ++ranges[4];
}

void GroupPointAsRectangle::MergeTwoRangeIntoOne(RectangleGroup& ranges, const RectangleGroup& ranges2) noexcept
{
    ranges[0] = std::min(ranges[0], ranges2[0]);
    ranges[1] = std::min(ranges[1], ranges2[1]);
    ranges[2] = std::max(ranges[2], ranges2[2]);
    ranges[3] = std::max(ranges[3], ranges2[3]);
    ranges[4] += ranges2[4];
}

GroupPointAsRectangle::RectangleGroup GroupPointAsRectangle::AddOneGroup(int x, int y) const noexcept
{
    return {x, y, x, y, 1};
}

bool GroupPointAsRectangle::TwoGroupIsNear(const RectangleGroup& group, const RectangleGroup& group2) const noexcept
{
    return PointNearRange(group2[0], group2[1], group, max_distance_)
        || PointNearRange(group2[2], group2[3], group, max_distance_);
}

void GroupPointAsRectangle::CheckAndMergeRangeIfItIsNear()
{
    for (std::size_t i = 0; i < rectangles_corners_.size(); ++i) {
        for (std::size_t j = i + 1; j < rectangles_corners_.size(); ++j) {
            if (!TwoGroupIsNear(rectangles_corners_[i], rectangles_corners_[j])) {
                continue;
            }

            MergeTwoRangeIntoOne(rectangles_corners_[i], rectangles_corners_[j]);
            rectangles_corners_.erase(rectangles_corners_.begin() + static_cast<std::ptrdiff_t>(j));
            CheckAndMergeRangeIfItIsNear();
            return;
        }
    }
}

} // namespace automationtest::opencvlib
