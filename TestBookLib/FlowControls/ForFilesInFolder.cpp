#include "ForFilesInFolder.hpp"

#include <filesystem>

namespace automationtest::testbooklib::flowcontrols {

ForFilesInFolder::ForFilesInFolder(int line_number, const std::string& text)
    : KeyValueLoop(line_number, text)
{
}

std::vector<std::string> ForFilesInFolder::GetList() const
{
    std::vector<std::string> result {};
    if (!std::filesystem::exists(parent_folder_name_)) {
        return result;
    }
    for (const auto& entry : std::filesystem::directory_iterator(parent_folder_name_)) {
        if (entry.is_regular_file()) {
            result.push_back(entry.path().string());
        }
    }
    return result;
}

} // namespace automationtest::testbooklib::flowcontrols
