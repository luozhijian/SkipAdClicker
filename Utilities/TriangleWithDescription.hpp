#ifndef AUTOMATIOTEST_TRIANGLEWITHDESCRIPTION_HPP
#define AUTOMATIOTEST_TRIANGLEWITHDESCRIPTION_HPP

#include "CommonTypes.hpp"
#include "PloygonWithDescription.hpp"
#include "eOrientation.hpp"

#include <string>
#include <vector>

namespace automationtest::utilities {

class TriangleWithDescription : public PloygonWithDescription {
public:
    TriangleWithDescription() = default;
    TriangleWithDescription(Point p1, Point p2, Point p3);

    [[nodiscard]] std::vector<Point> AsArray() const;
    [[nodiscard]] Rectangle GetCoveredRectangle() const;
    [[nodiscard]] bool IsEachEdgeTriangleLargeThan(int square_of_length = 9) const;
    [[nodiscard]] double Area() const;
    [[nodiscard]] TriangleWithDescription CreateAShift(const Size& shift) const;
    [[nodiscard]] Point Center() const noexcept;
    [[nodiscard]] bool GetVerticalOrHorizontalLine(int& which_line, eOrientation& orientation) const noexcept;
    [[nodiscard]] bool IsScrollbarCandidate();
    void Shift(const Size& size) noexcept;
    void CalculateAngles();
    [[nodiscard]] bool IsScrollBarOrDropDownArrow();
    [[nodiscard]] std::string ToString() const;

    Point point1 {};
    Point point2 {};
    Point point3 {};
    double angle_a {0.0};
    double angle_b {0.0};
    double angle_c {0.0};
    int scroll_bar_which_line {0};
    eOrientation scroll_bar_orientation {eOrientation::Horizontal};
    int scroll_bar_x_y_value {0};
    int scroll_bar_x_y_center {0};
    int scroll_bar_x_y_1 {0};
    int scroll_bar_x_y_2 {0};

private:
    static double AngleBetween(const Point& p1, const Point& vertex, const Point& p2);
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_TRIANGLEWITHDESCRIPTION_HPP
