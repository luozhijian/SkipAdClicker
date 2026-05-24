#ifndef AUTOMATIOTEST_OPENCVLIB_TRIANGLEFROMLINES_HPP
#define AUTOMATIOTEST_OPENCVLIB_TRIANGLEFROMLINES_HPP

#include "LineSegment2D.hpp"
#include "../Utilities/TriangleWithDescription.hpp"

#include <tuple>
#include <vector>

namespace automationtest::opencvlib {

class TriangleFromLines {
public:
    static std::vector<automationtest::utilities::TriangleWithDescription> FromLines(const std::vector<LineSegment2D>& lines);
};

} // namespace automationtest::opencvlib

#endif // AUTOMATIOTEST_OPENCVLIB_TRIANGLEFROMLINES_HPP
