#include "SmallTriangleDetector.hpp"

#include "TriangleDetector.hpp"

namespace automationtest::opencvlib {

using automationtest::utilities::Bitmap;
using automationtest::utilities::Rectangle;

SmallTriangleDetector::SmallTriangleDetector(const Bitmap& bitmap)
    : bitmap_(bitmap)
{
}

std::vector<Rectangle> SmallTriangleDetector::FindSmallTriangles() const
{
    std::vector<Rectangle> results {};
    const auto triangles = TriangleDetector(bitmap_).FindTriangles({});
    results.reserve(triangles.size());
    for (const auto& triangle : triangles) {
        const auto rect = triangle.GetCoveredRectangle();
        if (rect.width >= 5 && rect.height >= 5 && rect.width <= 30 && rect.height <= 30) {
            results.push_back(rect);
        }
    }
    return results;
}

} // namespace automationtest::opencvlib
