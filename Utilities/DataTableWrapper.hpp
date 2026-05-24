#ifndef AUTOMATIOTEST_DATATABLEWRAPPER_HPP
#define AUTOMATIOTEST_DATATABLEWRAPPER_HPP

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace automationtest::utilities {

class DataTableWrapper {
public:
    using Row = std::unordered_map<std::string, std::string>;

    DataTableWrapper& AddColumn(const std::string& column_name, int key);
    Row& GetNewRow();
    void AddDataToCurrentRow(int key, const std::string& value);
    void AddIntToCurrentRow(int key, int value);
    [[nodiscard]] const std::vector<Row>& Rows() const noexcept;

private:
    std::unordered_map<int, std::string> column_mapping_ {};
    std::vector<Row> rows_ {};
    std::optional<std::size_t> current_row_index_ {};
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_DATATABLEWRAPPER_HPP
