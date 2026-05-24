#include "CsvFileHelper.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace automationtest::utilities {

namespace {

std::vector<std::string> SplitCsvLine(const std::string& line)
{
    std::vector<std::string> result;
    std::string cell;
    bool in_quotes = false;

    for (std::size_t i = 0; i < line.size(); ++i) {
        const char ch = line[i];
        if (ch == '"') {
            if (in_quotes && i + 1 < line.size() && line[i + 1] == '"') {
                cell.push_back('"');
                ++i;
            } else {
                in_quotes = !in_quotes;
            }
        } else if (ch == ',' && !in_quotes) {
            result.push_back(cell);
            cell.clear();
        } else {
            cell.push_back(ch);
        }
    }
    result.push_back(cell);
    return result;
}

}

std::unordered_map<std::string, std::string> CsvFileHelper::Read2RowCsvFileAsDictionary(const std::string& file_path)
{
    std::ifstream input(file_path);
    std::string header_line;
    std::string data_line;
    if (!std::getline(input, header_line) || !std::getline(input, data_line)) {
        throw std::runtime_error("CSV file does not contain two rows.");
    }

    const auto headers = SplitCsvLine(header_line);
    const auto values = SplitCsvLine(data_line);
    if (headers.size() != values.size()) {
        throw std::runtime_error("Header and data row column counts do not match.");
    }

    std::unordered_map<std::string, std::string> result;
    for (std::size_t i = 0; i < headers.size(); ++i) {
        result[headers[i]] = values[i];
    }
    return result;
}

std::vector<CsvRow> CsvFileHelper::ReadCsvFileToRowList(const std::string& file_path)
{
    std::ifstream input(file_path);
    return ReadCsvStreamToRowList(input);
}

std::vector<CsvRow> CsvFileHelper::ReadCsvStringToRowList(const std::string& file_content)
{
    std::istringstream input(file_content);
    return ReadCsvStreamToRowList(input);
}

std::vector<CsvRow> CsvFileHelper::ReadCsvStreamToRowList(std::istream& stream)
{
    std::vector<CsvRow> records;
    std::string header_line;
    if (!std::getline(stream, header_line)) {
        return records;
    }

    const auto headers = SplitCsvLine(header_line);
    std::string ignored_types;
    std::getline(stream, ignored_types);

    std::string line;
    while (std::getline(stream, line)) {
        const auto values = SplitCsvLine(line);
        if (values.empty() || values[0].empty()) {
            continue;
        }

        CsvRow row;
        for (std::size_t i = 0; i < headers.size() && i < values.size(); ++i) {
            row[headers[i]] = values[i];
        }
        records.push_back(std::move(row));
    }
    return records;
}

} // namespace automationtest::utilities
