#include "LineWithDescription.hpp"

#include "RectangleHelper.hpp"

#include <cmath>
#include <sstream>

namespace automationtest::utilities {

LineWithDescription::LineWithDescription(Point p1, Point p2, unsigned char color_value)
    : point1_(p1)
    , point2_(p2)
    , color(color_value)
{
    Calculate();
}

const Point& LineWithDescription::Point1() const noexcept { return point1_; }
const Point& LineWithDescription::Point2() const noexcept { return point2_; }
const Point& LineWithDescription::Center() const noexcept { return center_; }
int LineWithDescription::Degree() const noexcept { return degree_; }

void LineWithDescription::Calculate() noexcept
{
    center_ = Point {(point1_.x + point2_.x) / 2, (point1_.y + point2_.y) / 2};
    degree_ = std::numeric_limits<int>::min();
    if (point1_.x == point2_.x) {
        degree_ = 90;
    } else if (point1_.y == point2_.y) {
        degree_ = 0;
    }
    length_.reset();
    length_square_.reset();
}

double LineWithDescription::Length() const
{
    if (!length_.has_value()) {
        length_ = std::sqrt(static_cast<double>(LengthSquare()));
    }
    return *length_;
}

int LineWithDescription::LengthSquare() const
{
    if (!length_square_.has_value()) {
        length_square_ = (point1_.x - point2_.x) * (point1_.x - point2_.x) + (point1_.y - point2_.y) * (point1_.y - point2_.y);
    }
    return *length_square_;
}

std::vector<Point> LineWithDescription::AsArray() const
{
    return {point1_, point2_};
}

std::string LineWithDescription::ToString() const
{
    std::ostringstream builder;
    builder << point1_.ToString() << ' ' << point2_.ToString() << " length: " << Length();
    return builder.str();
}

void LineWithDescription::AddConnection(LineWithDescription& line, int connection_type, bool is_point1, bool self_only)
{
    if (is_point1) {
        if (!point1_connections.has_value()) {
            point1_connections.emplace();
        }
        point1_connections->AddConnection(&line, connection_type);
        if (!self_only) {
            line.AddConnection(*this, connection_type == 1 ? 1 : 1, connection_type == 1, true);
        }
    } else {
        if (!point2_connections.has_value()) {
            point2_connections.emplace();
        }
        point2_connections->AddConnection(&line, connection_type);
        if (!self_only) {
            line.AddConnection(*this, connection_type == 1 ? 2 : 2, connection_type == 1, true);
        }
    }
}

bool LineWithDescription::IsExtend(const Point& point, int max_allowance) const noexcept
{
    return std::abs(point.x - center_.x) <= max_allowance || std::abs(point.y - center_.y) <= max_allowance;
}

bool LineWithDescription::Extend(Point point) noexcept
{
    point2_ = point;
    Calculate();
    return true;
}

bool LineWithDescription::IsXInRange(int x) const noexcept
{
    return (x >= point1_.x && x <= point2_.x) || (x <= point1_.x && x >= point2_.x);
}

bool LineWithDescription::IsYInRange(int y) const noexcept
{
    return (y >= point1_.y && y <= point2_.y) || (y <= point1_.y && y >= point2_.y);
}

bool LineWithDescription::IsVerticalLine(int shift) const noexcept
{
    return std::abs(point1_.x - point2_.x) <= shift;
}

bool LineWithDescription::IsHorizonLine(int shift) const noexcept
{
    return std::abs(point1_.y - point2_.y) <= shift;
}

Rectangle LineWithDescription::ToRectangle() const noexcept
{
    return RectangleHelper::FromTwoPoints(point1_, point2_);
}

} // namespace automationtest::utilities
