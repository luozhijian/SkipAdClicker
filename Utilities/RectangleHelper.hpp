#ifndef AUTOMATIOTEST_RECTANGLEHELPER_HPP
#define AUTOMATIOTEST_RECTANGLEHELPER_HPP

#include "CommonTypes.hpp"
#include "Types/EnumRelativeLocation.hpp"
#include "Types/IntPair.hpp"
#include "eOrientation.hpp"

#include <optional>
#include <regex>
#include <tuple>
#include <vector>

namespace automationtest::utilities {

class LineWithDescription;

class RectangleHelper {
public:
    static Rectangle Shift(const Rectangle& rect, const Point& shift) noexcept;
    static Rectangle FindContainerRectangle(const std::vector<Rectangle>& rectangles, const Rectangle& rectangle, bool near_edge = true, eOrientation orientation = eOrientation::Horizontal) noexcept;
    static Rectangle CreateRectangleFromLines(const std::vector<LineWithDescription*>& lines) noexcept;
    static Rectangle FromTwoPoints(const Point& p1, const Point& p2) noexcept;
    static Rectangle FromPoints(const std::vector<Point>& points) noexcept;
    static Rectangle FromSizeAnd0(const Size& size) noexcept;
    static Rectangle FromXYXY(int x1, int y1, int x2, int y2) noexcept;
    static bool IsHorizonOrVerticalLine(const Point& p1, const Point& p2) noexcept;
    static bool FirstRectangleIsAtRightOfSecondSameRow(const Rectangle& left_rectangle, const Rectangle& right_rectangle) noexcept;
    static bool TwoRectangleInSameRow(const Rectangle& rect1, const Rectangle& rect2) noexcept;
    static bool TwoRectangleInSameColumn(const Rectangle& rect1, const Rectangle& rect2) noexcept;
    static Point Center(const Rectangle& rectangle) noexcept;
    static Point GetPoint(const Rectangle& rectangle, double x_percent, double y_percent) noexcept;
    static int Area(const Rectangle& rectangle) noexcept;
    static Rectangle IncreaseWidthHeight(Rectangle rectangle, int width, int height) noexcept;
    static Rectangle MakeRectangle(const types::IntPair& xs, const types::IntPair& ys, int delta_x = 0, int delta_y = 0) noexcept;
    static bool CheckRelativeLocation(const Rectangle& fix_rectangle, const Rectangle& relative_rectangle, types::EnumRelativeLocation relative_location, int possible_letter_height = 20000, int scale = 3);
    static bool PoinInOrOneLineAwayFromRectangle_Y(const Rectangle& fix_rectangle, const Point& point, int one_line_height) noexcept;
    static std::optional<Rectangle> ToRectangle(const std::string& value);
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_RECTANGLEHELPER_HPP
