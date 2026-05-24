#ifndef AUTOMATIOTEST_USERINTERFACELIB_LAYOUT_LAYOUTDETECTOR_HPP
#define AUTOMATIOTEST_USERINTERFACELIB_LAYOUT_LAYOUTDETECTOR_HPP

#include "../../Utilities/Common.hpp"

#include <vector>

namespace automationtest::userinterfacelib {

class LayoutDetector {
public:
    explicit LayoutDetector(const Bitmap& bitmap);
    LayoutDetector(const std::vector<std::byte>& gray_values, int stride, int width, int height);

    [[nodiscard]] Rectangle GetContentRectangle(int min_length = 200) const;
    static Rectangle FindTopPart(const Bitmap& bitmap);
    static std::vector<Rectangle> AdjustLayoutAs(const Rectangle& popup, const std::vector<Rectangle>& rectangles, int items_per_row);
    static std::vector<TextMatch> FindTextWithSpecificLayout(const Bitmap& bitmap, int items_per_row, int min_size, const TextProvider& provider = {});
    [[nodiscard]] Rectangle FigureSizeFromLocation(const std::vector<std::string>& locations, const Size& container_size) const;

private:
    std::vector<std::byte> gray_values_ {};
    int stride_ {0};
    int width_ {0};
    int height_ {0};
    Bitmap bitmap_ {};
};

} // namespace automationtest::userinterfacelib

#endif // AUTOMATIOTEST_USERINTERFACELIB_LAYOUT_LAYOUTDETECTOR_HPP
