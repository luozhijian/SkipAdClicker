#ifndef AUTOMATIOTEST_OPENCVLIB_TRIANGLEDETECTOR2_HPP
#define AUTOMATIOTEST_OPENCVLIB_TRIANGLEDETECTOR2_HPP

#include "../Utilities/TriangleWithDescription.hpp"

#include <cstddef>
#include <vector>

namespace automationtest::opencvlib {

class TriangleDetector2 {
public:
    TriangleDetector2(const std::vector<std::byte>& gray_values, int stride, int height, int width);

    [[nodiscard]] std::vector<automationtest::utilities::TriangleWithDescription> FindTriangles() const;

private:
    std::vector<std::byte> values_ {};
    int stride_ {0};
    int height_ {0};
    int width_ {0};
};

} // namespace automationtest::opencvlib

#endif // AUTOMATIOTEST_OPENCVLIB_TRIANGLEDETECTOR2_HPP
