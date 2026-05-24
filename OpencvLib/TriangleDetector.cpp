#include "TriangleDetector.hpp"

#include "CornerDetector.hpp"

#include "../Utilities/BitmapHelper.hpp"

#include <queue>
#include <set>

namespace automationtest::opencvlib {

using automationtest::utilities::Bitmap;
using automationtest::utilities::BitmapHelper;
using automationtest::utilities::Point;
using automationtest::utilities::TriangleWithDescription;

namespace {

std::uint8_t GrayAt(const Bitmap& bitmap, int row, int col)
{
    const auto index = static_cast<std::size_t>(row) * static_cast<std::size_t>(bitmap.stride) + static_cast<std::size_t>(col);
    return static_cast<std::uint8_t>(bitmap.pixels[index]);
}

} // namespace

TriangleDetector::TriangleDetector(const Bitmap& bitmap)
    : bitmap_(BitmapHelper::ConvertToGrayscale(bitmap))
{
}

std::vector<TriangleWithDescription> TriangleDetector::FindTriangles(const automationtest::utilities::settings::SettingTriangleDetection& triangle_setting) const
{
    std::vector<TriangleWithDescription> triangles {};
    if (bitmap_.width <= 0 || bitmap_.height <= 0 || bitmap_.pixels.empty()) {
        return triangles;
    }

    std::vector<std::uint8_t> visited(static_cast<std::size_t>(bitmap_.width) * static_cast<std::size_t>(bitmap_.height), 0);
    CornerDetector detector {};

    const auto mark_index = [this](int row, int col) {
        return static_cast<std::size_t>(row) * static_cast<std::size_t>(bitmap_.width) + static_cast<std::size_t>(col);
    };

    for (int row = 0; row < bitmap_.height; ++row) {
        for (int col = 0; col < bitmap_.width; ++col) {
            const auto seed = GrayAt(bitmap_, row, col);
            if (seed < triangle_setting.in_range_lower || seed > triangle_setting.in_range_upper) {
                continue;
            }

            if (visited[mark_index(row, col)] != 0) {
                continue;
            }

            std::queue<Point> queue {};
            std::vector<Point> region {};
            queue.push(Point {col, row});
            visited[mark_index(row, col)] = 1;

            while (!queue.empty()) {
                const auto point = queue.front();
                queue.pop();
                region.push_back(point);
                if (static_cast<int>(region.size()) > triangle_setting.max_area) {
                    region.clear();
                    break;
                }

                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0) {
                            continue;
                        }
                        const auto next_x = point.x + dx;
                        const auto next_y = point.y + dy;
                        if (next_x < 0 || next_y < 0 || next_x >= bitmap_.width || next_y >= bitmap_.height) {
                            continue;
                        }

                        const auto idx = mark_index(next_y, next_x);
                        if (visited[idx] != 0) {
                            continue;
                        }

                        const auto value = GrayAt(bitmap_, next_y, next_x);
                        if (value < triangle_setting.in_range_lower || value > triangle_setting.in_range_upper) {
                            continue;
                        }

                        visited[idx] = 1;
                        queue.push(Point {next_x, next_y});
                    }
                }
            }

            if (region.size() < static_cast<std::size_t>(triangle_setting.min_area)) {
                continue;
            }

            const auto corners = detector.Detect(region);
            if (corners.size() != 3) {
                continue;
            }

            TriangleWithDescription triangle(corners[0], corners[1], corners[2]);
            if (triangle.IsScrollBarOrDropDownArrow()) {
                triangles.push_back(triangle);
            }
        }
    }

    return triangles;
}

} // namespace automationtest::opencvlib
