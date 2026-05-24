#ifndef AUTOMATIOTEST_USERINTERFACELIB_GRID_GRIDSTYLE_HPP
#define AUTOMATIOTEST_USERINTERFACELIB_GRID_GRIDSTYLE_HPP

#include "../../Utilities/Common.hpp"

namespace automationtest::userinterfacelib {

class GridStyle {
public:
    Bitmap filter_icon {};
    Bitmap column_separator_in_header {};
    Bitmap more_icon {};
    bool with_row_color {false};
};

} // namespace automationtest::userinterfacelib

#endif // AUTOMATIOTEST_USERINTERFACELIB_GRID_GRIDSTYLE_HPP
