#include "ReadFileAsValuePairList.hpp"

#include <fstream>

namespace automationtest::utilities::file_path {

std::vector<std::pair<std::string, std::string>> ReadFileAsValuePairList::Read(const std::string& file_path, const std::string& split)
{
    std::vector<std::pair<std::string, std::string>> result;
    std::ifstream input(file_path);
    std::string line;

    while (std::getline(input, line)) {
        const auto separator = line.find(split);
        if (separator == std::string::npos) {
            continue;
        }

        result.emplace_back(line.substr(0, separator), line.substr(separator + split.size()));
    }

    return result;
}

} // namespace automationtest::utilities::file_path
