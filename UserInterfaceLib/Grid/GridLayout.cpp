#include "GridLayout.hpp"

#include "../../Utilities/RectangleHelper.hpp"

namespace automationtest::userinterfacelib {

Rectangle GridLayout::GetOneCell(int row, int col) const noexcept
{
    if (row < 0 || col < 0 || static_cast<std::size_t>(row) >= rows.size() || static_cast<std::size_t>(col) >= columns.size()) {
        return {};
    }
    return automationtest::utilities::RectangleHelper::MakeRectangle(columns[static_cast<std::size_t>(col)], rows[static_cast<std::size_t>(row)]);
}

} // namespace automationtest::userinterfacelib
