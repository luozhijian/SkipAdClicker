#include "ApplicationDataManager.hpp"

#include "../FileHelper.hpp"
#include "../FilePathLib.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace automationtest::utilities::status {

std::string ApplicationDataManager::GetFullfilenameInApplicationData(const std::string& filename)
{
    const char* app_data = std::getenv("APPDATA");
    const std::filesystem::path folder = (app_data != nullptr ? std::filesystem::path(app_data) : std::filesystem::current_path()) / application_name;
    FilePathLib::CreateFolderIfNotExists(folder.string());
    return (folder / filename).string();
}

std::string ApplicationDataManager::ReadFileInApplicationDataFolder(const std::string& filename)
{
    const auto path = GetFullfilenameInApplicationData(filename);
    if (!std::filesystem::exists(path)) {
        return {};
    }
    return FileHelper::FileReadAllText(path);
}

bool ApplicationDataManager::WriteFileInApplicationDataFolder(const std::string& filename, const std::string& content)
{
    const auto path = GetFullfilenameInApplicationData(filename);
    std::ofstream output(path, std::ios::binary);
    output << content;
    return true;
}

} // namespace automationtest::utilities::status
