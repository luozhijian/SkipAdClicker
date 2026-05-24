#ifndef AUTOMATIOTEST_LINEWITHDESCRIPTION_HPP
#define AUTOMATIOTEST_LINEWITHDESCRIPTION_HPP

#include "CommonTypes.hpp"
#include "PloygonConnections.hpp"
#include "PloygonWithDescription.hpp"

#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace automationtest::utilities {

class LineWithDescription : public PloygonWithDescription {
public:
    LineWithDescription() = default;
    LineWithDescription(Point p1, Point p2, unsigned char color = 0);

    [[nodiscard]] const Point& Point1() const noexcept;
    [[nodiscard]] const Point& Point2() const noexcept;
    [[nodiscard]] const Point& Center() const noexcept;
    [[nodiscard]] int Degree() const noexcept;
    [[nodiscard]] double Length() const;
    [[nodiscard]] int LengthSquare() const;
    [[nodiscard]] std::vector<Point> AsArray() const;
    [[nodiscard]] std::string ToString() const;
    void AddConnection(LineWithDescription& line, int connection_type, bool is_point1, bool self_only = false);
    bool IsExtend(const Point& point, int max_allowance) const noexcept;
    bool Extend(Point point) noexcept;
    bool IsXInRange(int x) const noexcept;
    bool IsYInRange(int y) const noexcept;
    bool IsVerticalLine(int shift = 2) const noexcept;
    bool IsHorizonLine(int shift = 2) const noexcept;
    Rectangle ToRectangle() const noexcept;

    int thickness {0};
    std::vector<Point> real_points {};
    unsigned char color {0};
    std::optional<PloygonConnections<LineWithDescription>> point1_connections {};
    std::optional<PloygonConnections<LineWithDescription>> point2_connections {};

private:
    void Calculate() noexcept;

    Point point1_ {};
    Point point2_ {};
    Point center_ {};
    int degree_ {std::numeric_limits<int>::min()};
    mutable std::optional<double> length_ {};
    mutable std::optional<int> length_square_ {};
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_LINEWITHDESCRIPTION_HPP
