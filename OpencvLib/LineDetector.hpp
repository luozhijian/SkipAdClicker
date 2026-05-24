#ifndef AUTOMATIOTEST_OPENCVLIB_LINEDETECTOR_HPP
#define AUTOMATIOTEST_OPENCVLIB_LINEDETECTOR_HPP

#include "../Utilities/CommonTypes.hpp"
#include "../Utilities/LineWithDescription.hpp"

#include <set>
#include <vector>

namespace automationtest::opencvlib {

class LineDetector {
public:
    explicit LineDetector(const automationtest::utilities::Bitmap& bitmap);
    LineDetector(const std::vector<std::byte>& gray_values, int stride, int width, int height, std::uint8_t background_color = 0);

    [[nodiscard]] std::vector<automationtest::utilities::LineWithDescription> FindLines(int threshold = 40) const;
    [[nodiscard]] std::vector<automationtest::utilities::LineWithDescription> FindAllHorizonLines(int threshold = 40, bool explicit_line = true) const;
    [[nodiscard]] std::vector<automationtest::utilities::LineWithDescription> FindAllVerticalLines(int threshold = 40, bool explicit_line = true) const;

private:
    [[nodiscard]] std::uint8_t PixelAt(int row, int column) const;
    [[nodiscard]] bool InBounds(int row, int column) const noexcept;

    std::vector<std::byte> values_ {};
    int stride_ {0};
    int width_ {0};
    int height_ {0};
    std::uint8_t background_color_ {0};
};

} // namespace automationtest::opencvlib

#endif // AUTOMATIOTEST_OPENCVLIB_LINEDETECTOR_HPP
