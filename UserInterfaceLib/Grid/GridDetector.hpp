#ifndef AUTOMATIOTEST_USERINTERFACELIB_GRID_GRIDDETECTOR_HPP
#define AUTOMATIOTEST_USERINTERFACELIB_GRID_GRIDDETECTOR_HPP

#include "GridLayout.hpp"
#include "GridStyle.hpp"

namespace automationtest::userinterfacelib {

class GridDetector {
public:
    GridDetector(Bitmap bitmap, Rectangle rect);

    static GridDetector FindGrid(Bitmap bitmap, Rectangle rect, const std::string& grid_style, int threshold_x, int threshold_y);
    GridLayout Detector2(int threshold_x, int threshold_y, const TextProvider& provider = {}) const;

private:
    GridLayout ReadGrid(const Bitmap& clipped_bitmap, const std::vector<automationtest::utilities::types::IntPair>& row_pairs, const std::vector<automationtest::utilities::types::IntPair>& column_pairs, const TextProvider& provider) const;

    Bitmap bitmap_ {};
    Rectangle rect_ {};
    GridStyle grid_detector_resource_ {};
};

} // namespace automationtest::userinterfacelib

#endif // AUTOMATIOTEST_USERINTERFACELIB_GRID_GRIDDETECTOR_HPP
