#ifndef AUTOMATIOTEST_CSVFILEHELPER_HPP
#define AUTOMATIOTEST_CSVFILEHELPER_HPP

#include <istream>
#include <string>
#include <unordered_map>
#include <vector>

namespace automationtest::utilities {

using CsvRow = std::unordered_map<std::string, std::string>;

class CsvFileHelper {
public:
    static std::unordered_map<std::string, std::string> Read2RowCsvFileAsDictionary(const std::string& file_path);
    static std::vector<CsvRow> ReadCsvFileToRowList(const std::string& file_path);
    static std::vector<CsvRow> ReadCsvStringToRowList(const std::string& file_content);

private:
    static std::vector<CsvRow> ReadCsvStreamToRowList(std::istream& stream);
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_CSVFILEHELPER_HPP
