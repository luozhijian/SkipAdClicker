#include "DataTableWrapper.hpp"

#include <stdexcept>

namespace automationtest::utilities {

DataTableWrapper& DataTableWrapper::AddColumn(const std::string& column_name, int key)
{
    column_mapping_[key] = column_name;
    return *this;
}

DataTableWrapper::Row& DataTableWrapper::GetNewRow()
{
    rows_.emplace_back();
    current_row_index_ = rows_.size() - 1;
    return rows_.back();
}

void DataTableWrapper::AddDataToCurrentRow(int key, const std::string& value)
{
    if (!current_row_index_.has_value()) {
        throw std::runtime_error("Current row is not initialized");
    }
    const auto it = column_mapping_.find(key);
    if (it == column_mapping_.end()) {
        throw std::runtime_error("Cannot find column key");
    }
    rows_[*current_row_index_][it->second] = value;
}

void DataTableWrapper::AddIntToCurrentRow(int key, int value)
{
    AddDataToCurrentRow(key, std::to_string(value));
}

const std::vector<DataTableWrapper::Row>& DataTableWrapper::Rows() const noexcept
{
    return rows_;
}

} // namespace automationtest::utilities
