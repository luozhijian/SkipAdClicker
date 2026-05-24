#include "RectangleDetector.hpp"

#include "../Utilities/PointHelper.hpp"
#include "../Utilities/RectangleHelper.hpp"

#include <set>

namespace automationtest::opencvlib {

using automationtest::utilities::LineWithDescription;
using automationtest::utilities::Point;
using automationtest::utilities::PointHelper;
using automationtest::utilities::Rectangle;
using automationtest::utilities::RectangleHelper;

std::vector<Rectangle> RectangleDetector::ExtractRectanglesFromLines(const std::vector<LineWithDescription>& lines, double delta_x, double delta_y)
{
    std::vector<LineWithDescription> verticals {};
    std::vector<LineWithDescription> horizontals {};

    for (const auto& line : lines) {
        if (line.IsVerticalLine() && line.Length() >= 12.0) {
            verticals.push_back(line);
        } else if (line.IsHorizonLine() && line.Length() >= 20.0) {
            horizontals.push_back(line);
        }
    }

    std::set<std::tuple<int, int, int, int>> unique {};
    std::vector<Rectangle> rectangles {};

    for (std::size_t left_index = 0; left_index < verticals.size(); ++left_index) {
        for (std::size_t right_index = left_index + 1; right_index < verticals.size(); ++right_index) {
            const auto left_x = verticals[left_index].Point1().x;
            const auto right_x = verticals[right_index].Point1().x;
            if (right_x <= left_x) {
                continue;
            }

            for (std::size_t top_index = 0; top_index < horizontals.size(); ++top_index) {
                for (std::size_t bottom_index = top_index + 1; bottom_index < horizontals.size(); ++bottom_index) {
                    const auto top_y = horizontals[top_index].Point1().y;
                    const auto bottom_y = horizontals[bottom_index].Point1().y;
                    if (bottom_y <= top_y) {
                        continue;
                    }

                    const auto left_ok = verticals[left_index].IsYInRange(top_y - static_cast<int>(delta_y))
                        && verticals[left_index].IsYInRange(bottom_y + static_cast<int>(delta_y));
                    const auto right_ok = verticals[right_index].IsYInRange(top_y - static_cast<int>(delta_y))
                        && verticals[right_index].IsYInRange(bottom_y + static_cast<int>(delta_y));
                    const auto top_ok = horizontals[top_index].IsXInRange(left_x - static_cast<int>(delta_x))
                        && horizontals[top_index].IsXInRange(right_x + static_cast<int>(delta_x));
                    const auto bottom_ok = horizontals[bottom_index].IsXInRange(left_x - static_cast<int>(delta_x))
                        && horizontals[bottom_index].IsXInRange(right_x + static_cast<int>(delta_x));

                    if (!(left_ok && right_ok && top_ok && bottom_ok)) {
                        continue;
                    }

                    auto rectangle = RectangleHelper::FromXYXY(left_x, top_y, right_x, bottom_y);
                    const auto key = std::make_tuple(rectangle.x, rectangle.y, rectangle.width, rectangle.height);
                    if (unique.insert(key).second) {
                        rectangles.push_back(rectangle);
                    }
                }
            }
        }
    }

    return rectangles;
}

} // namespace automationtest::opencvlib
