#include "RectangleHelper.hpp"

#include "LineWithDescription.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace automationtest::utilities {

Rectangle RectangleHelper::Shift(const Rectangle& rect, const Point& shift) noexcept
{
    return Rectangle {rect.x + shift.x, rect.y + shift.y, rect.width, rect.height};
}

Rectangle RectangleHelper::FindContainerRectangle(const std::vector<Rectangle>& rectangles, const Rectangle& rectangle, bool near_edge, eOrientation orientation) noexcept
{
    Rectangle result {};
    bool found = false;
    for (const auto& rect : rectangles) {
        const bool contains =
            rect.Left() <= rectangle.Left() && rect.Top() <= rectangle.Top()
            && rect.Right() >= rectangle.Right() && rect.Bottom() >= rectangle.Bottom();
        if (!contains) {
            continue;
        }

        if (near_edge) {
            if (orientation == eOrientation::Vertical) {
                const int x_diff = rect.Right() - rectangle.Right();
                const int y_diff_upper = rectangle.Top() - rect.Top();
                const int y_diff_bottom = rect.Bottom() - rectangle.Bottom();
                if (!(x_diff > 5 && y_diff_upper < 20 && y_diff_bottom < 20)) {
                    continue;
                }
            } else {
                const int y_diff = rect.Bottom() - rectangle.Bottom();
                const int x_diff_left = rectangle.Left() - rect.Left();
                const int x_diff_right = rect.Right() - rectangle.Right();
                if (!(y_diff > 5 && x_diff_left < 20 && x_diff_right < 20)) {
                    continue;
                }
            }
        }

        if (!found || (result.Left() <= rect.Left() && result.Top() <= rect.Top() && result.Right() >= rect.Right() && result.Bottom() >= rect.Bottom())) {
            result = rect;
            found = true;
        }
    }
    return result;
}

Rectangle RectangleHelper::CreateRectangleFromLines(const std::vector<LineWithDescription*>& lines) noexcept
{
    int top_left_x = std::numeric_limits<int>::max();
    int top_left_y = std::numeric_limits<int>::max();
    int bottom_right_x = std::numeric_limits<int>::min();
    int bottom_right_y = std::numeric_limits<int>::min();

    for (const auto* line : lines) {
        if (line == nullptr) {
            continue;
        }
        top_left_x = std::min({top_left_x, line->Point1().x, line->Point2().x});
        top_left_y = std::min({top_left_y, line->Point1().y, line->Point2().y});
        bottom_right_x = std::max({bottom_right_x, line->Point1().x, line->Point2().x});
        bottom_right_y = std::max({bottom_right_y, line->Point1().y, line->Point2().y});
    }

    return Rectangle {top_left_x, top_left_y, bottom_right_x - top_left_x, bottom_right_y - top_left_y};
}

Rectangle RectangleHelper::FromTwoPoints(const Point& p1, const Point& p2) noexcept
{
    return FromXYXY(p1.x, p1.y, p2.x, p2.y);
}

Rectangle RectangleHelper::FromPoints(const std::vector<Point>& points) noexcept
{
    int x_min = std::numeric_limits<int>::max();
    int x_max = std::numeric_limits<int>::min();
    int y_min = std::numeric_limits<int>::max();
    int y_max = std::numeric_limits<int>::min();
    for (const auto& point : points) {
        x_min = std::min(x_min, point.x);
        x_max = std::max(x_max, point.x);
        y_min = std::min(y_min, point.y);
        y_max = std::max(y_max, point.y);
    }
    return Rectangle {x_min, y_min, x_max - x_min, y_max - y_min};
}

Rectangle RectangleHelper::FromSizeAnd0(const Size& size) noexcept
{
    return Rectangle {0, 0, size.width, size.height};
}

Rectangle RectangleHelper::FromXYXY(int x1, int y1, int x2, int y2) noexcept
{
    const int x_min = std::min(x1, x2);
    const int x_max = std::max(x1, x2);
    const int y_min = std::min(y1, y2);
    const int y_max = std::max(y1, y2);
    return Rectangle {x_min, y_min, x_max - x_min, y_max - y_min};
}

bool RectangleHelper::IsHorizonOrVerticalLine(const Point& p1, const Point& p2) noexcept
{
    return std::abs(p1.x - p2.x) <= 2 || std::abs(p1.y - p2.y) <= 2;
}

bool RectangleHelper::FirstRectangleIsAtRightOfSecondSameRow(const Rectangle& left_rectangle, const Rectangle& right_rectangle) noexcept
{
    const int first_mid_y = left_rectangle.Top() + left_rectangle.height / 2;
    const int second_mid_y = right_rectangle.Top() + right_rectangle.height / 2;
    const int min_y = std::max(left_rectangle.Top(), right_rectangle.Top());
    const int max_y = std::min(left_rectangle.Bottom(), right_rectangle.Bottom());

    if (!(first_mid_y >= min_y && first_mid_y <= max_y && second_mid_y >= min_y && second_mid_y <= max_y)) {
        return false;
    }

    return left_rectangle.Left() < right_rectangle.Left();
}

bool RectangleHelper::TwoRectangleInSameRow(const Rectangle& rect1, const Rectangle& rect2) noexcept
{
    return rect1.Top() <= rect2.Bottom() && rect2.Top() <= rect1.Bottom();
}

bool RectangleHelper::TwoRectangleInSameColumn(const Rectangle& rect1, const Rectangle& rect2) noexcept
{
    return rect1.Left() <= rect2.Right() && rect2.Left() <= rect1.Right();
}

Point RectangleHelper::Center(const Rectangle& rectangle) noexcept
{
    return Point {rectangle.Left() + rectangle.width / 2, rectangle.Top() + rectangle.height / 2};
}

Point RectangleHelper::GetPoint(const Rectangle& rectangle, double x_percent, double y_percent) noexcept
{
    return Point {
        static_cast<int>(rectangle.Left() + rectangle.width * x_percent),
        static_cast<int>(rectangle.Top() + rectangle.height * y_percent)
    };
}

int RectangleHelper::Area(const Rectangle& rectangle) noexcept
{
    return rectangle.width * rectangle.height;
}

Rectangle RectangleHelper::IncreaseWidthHeight(Rectangle rectangle, int width, int height) noexcept
{
    rectangle.width += width;
    rectangle.height += height;
    return rectangle;
}

Rectangle RectangleHelper::MakeRectangle(const types::IntPair& xs, const types::IntPair& ys, int delta_x, int delta_y) noexcept
{
    return Rectangle {xs.First(), ys.First(), xs.Length() - delta_x, ys.Length() - delta_y};
}

bool RectangleHelper::CheckRelativeLocation(const Rectangle& fix_rectangle, const Rectangle& relative_rectangle, types::EnumRelativeLocation relative_location, int possible_letter_height, int scale)
{
    if (fix_rectangle == relative_rectangle) {
        return false;
    }

    switch (relative_location) {
    case types::EnumRelativeLocation::Right:
        return TwoRectangleInSameRow(fix_rectangle, relative_rectangle) && relative_rectangle.Left() <= fix_rectangle.Right();
    case types::EnumRelativeLocation::Left:
        return TwoRectangleInSameRow(fix_rectangle, relative_rectangle) && relative_rectangle.Right() > fix_rectangle.Left();
    case types::EnumRelativeLocation::SameRowLeftClose:
        return TwoRectangleInSameRow(fix_rectangle, relative_rectangle)
            && std::abs(relative_rectangle.Left() - fix_rectangle.Left()) < possible_letter_height * scale;
    case types::EnumRelativeLocation::SameRowRightClose:
        return TwoRectangleInSameRow(fix_rectangle, relative_rectangle)
            && std::abs(relative_rectangle.Right() - fix_rectangle.Right()) < possible_letter_height * scale;
    case types::EnumRelativeLocation::Below: {
        const int diff = relative_rectangle.Top() - fix_rectangle.Bottom();
        return diff > 0 && diff < possible_letter_height * scale;
    }
    default:
        throw std::runtime_error("Unsupported relative location");
    }
}

bool RectangleHelper::PoinInOrOneLineAwayFromRectangle_Y(const Rectangle& fix_rectangle, const Point& point, int one_line_height) noexcept
{
    return point.y >= fix_rectangle.Top() - one_line_height && point.y <= fix_rectangle.Bottom() + one_line_height;
}

std::optional<Rectangle> RectangleHelper::ToRectangle(const std::string& value)
{
    if (value.empty()) {
        return std::nullopt;
    }

    static const std::regex pattern(R"(Rectangle\s*\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\))", std::regex_constants::icase);
    std::smatch match;
    if (!std::regex_match(value, match, pattern)) {
        throw std::runtime_error("Unknown Rectangle: " + value);
    }
    return Rectangle {std::stoi(match[1].str()), std::stoi(match[2].str()), std::stoi(match[3].str()), std::stoi(match[4].str())};
}

} // namespace automationtest::utilities
