#ifndef AUTOMATIOTEST_USERINTERFACELIB_GRID_GRIDLAYOUT_HPP
#define AUTOMATIOTEST_USERINTERFACELIB_GRID_GRIDLAYOUT_HPP

#include "../../Utilities/Common.hpp"
#include "../../Utilities/DataTableWrapper.hpp"
#include "../../Utilities/Types/IntPair.hpp"

#include <string>
#include <vector>

namespace automationtest::userinterfacelib {

class GridLayout {
public:
    [[nodiscard]] Rectangle GetOneCell(int row, int col) const noexcept;

    Rectangle grid_location {};
    std::string name {};
    std::string description {};
    std::vector<automationtest::utilities::types::IntPair> rows {};
    std::vector<automationtest::utilities::types::IntPair> columns {};
    std::vector<std::string> headers {};
    automationtest::utilities::DataTableWrapper data_table_wrapper {};
    Bitmap bitmap {};
    std::string layout {};
    std::string layout_part {};
    std::string grid_style {};
};

} // namespace automationtest::userinterfacelib

#endif // AUTOMATIOTEST_USERINTERFACELIB_GRID_GRIDLAYOUT_HPP
