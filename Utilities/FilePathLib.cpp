#include "FilePathLib.hpp"

#include "IdGenerator.hpp"
#include "Logger.hpp"
#include "GlobalSetting.hpp"

#include <filesystem>
#include <fstream>

namespace automationtest::utilities {

namespace fs = std::filesystem;

void FilePathLib::CreateFolderIfNotExists(const std::string& path)
{
    if (!path.empty()) {
        fs::create_directories(path);
    }
}

std::string FilePathLib::PathCombine(const std::string& path1, const std::string& path2, const std::string& path3, const std::string& path4)
{
    fs::path result = fs::path(path1) / path2;
    if (!path3.empty()) {
        result /= path3;
    }
    if (!path4.empty()) {
        result /= path4;
    }
    return result.string();
}

void FilePathLib::CopyFileWithName(const std::string& source, const std::string& destination, bool overwrite)
{
    const auto parent = fs::path(destination).parent_path();
    if (!parent.empty()) {
        CreateFolderIfNotExists(parent.string());
    }
    fs::copy_file(source, destination, overwrite ? fs::copy_options::overwrite_existing : fs::copy_options::none);
}

int FilePathLib::CopyFilesToAnotherFolder(const std::string& source_folder, const std::string& destination_folder, const std::string&, bool overwrite)
{
    int copied = 0;
    if (!fs::exists(source_folder)) {
        return 0;
    }

    CreateFolderIfNotExists(destination_folder);
    for (const auto& entry : fs::directory_iterator(source_folder)) {
        if (!entry.is_regular_file()) {
            continue;
        }
        fs::copy_file(entry.path(), fs::path(destination_folder) / entry.path().filename(), overwrite ? fs::copy_options::overwrite_existing : fs::copy_options::none);
        ++copied;
    }
    return copied;
}

bool FilePathLib::CanRemoveFileSuccessfully(const std::string& destination_file)
{
    std::error_code error;
    return fs::remove(destination_file, error) && !error;
}

std::string FilePathLib::CopyFileToFolder(const std::string& source, const std::string& destination_path, bool overwrite)
{
    CreateFolderIfNotExists(destination_path);
    const auto destination = fs::path(destination_path) / fs::path(source).filename();
    fs::copy_file(source, destination, overwrite ? fs::copy_options::overwrite_existing : fs::copy_options::none);
    return destination.string();
}

int FilePathLib::RemoveAllFileInFolder(const std::string& destination_path, const std::string&, int max_count_of_file_to_remove)
{
    std::vector<fs::path> files;
    for (const auto& entry : fs::directory_iterator(destination_path)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path());
        }
    }

    if (static_cast<int>(files.size()) > max_count_of_file_to_remove) {
        return -1;
    }

    for (const auto& file : files) {
        fs::remove(file);
    }
    return static_cast<int>(files.size());
}

int FilePathLib::TryRemoveAllFileInFolder(const std::string& destination_path, const std::string& pattern, int max_count_of_file_to_remove)
{
    try {
        return RemoveAllFileInFolder(destination_path, pattern, max_count_of_file_to_remove);
    } catch (...) {
        return -1;
    }
}

bool FilePathLib::CheckIfFileExists(const std::string& source)
{
    return fs::exists(source);
}

std::string FilePathLib::GenerateTestingOutputFilename(const std::string& filename, const std::string& function_name, const std::string& extension)
{
    const fs::path input(filename);
    const auto output_path = input.parent_path() / "TempData";
    CreateFolderIfNotExists(output_path.string());
    return (output_path / (input.filename().string() + "_" + function_name + extension)).string();
}

std::string FilePathLib::AddCountToPathName(const std::string& filename)
{
    const auto extension_pos = filename.find_last_of('.');
    if (extension_pos == std::string::npos) {
        return filename;
    }
    return filename.substr(0, extension_pos) + "_" + std::to_string(IdGenerator::Id()) + filename.substr(extension_pos);
}

std::string FilePathLib::GenerateOutputResult(const std::string& filename, const std::string& prefix)
{
    const fs::path input(filename);
    const auto output = input.parent_path() / "TempData";
    CreateFolderIfNotExists(output.string());
    return (output / (prefix + "_" + input.filename().string())).string();
}

void FilePathLib::MoveFilesWithPrefix(const std::string& source_folder, const std::string& destination_folder, const std::string& prefix)
{
    CreateFolderIfNotExists(destination_folder);
    for (const auto& entry : fs::directory_iterator(source_folder)) {
        if (!entry.is_regular_file()) {
            continue;
        }
        const auto destination = fs::path(destination_folder) / (prefix + entry.path().filename().string());
        if (fs::exists(destination)) {
            fs::remove(destination);
        }
        fs::rename(entry.path(), destination);
    }
}

bool FilePathLib::SafeSaveToFile(const std::string& file_path, const std::string& content)
{
    try {
        std::ofstream stream(file_path, std::ios::app | std::ios::binary);
        stream << content;
        return true;
    } catch (...) {
        return false;
    }
}

void FilePathLib::SaveToFile(const std::string& file_path, const std::string& content)
{
 
    std::ofstream stream(file_path, std::ios::app | std::ios::binary);
    stream << content;
}


std::vector<std::string> FilePathLib::FindFilesEndingWith(const std::string& folder_path, const std::string& search_string)
{
    std::vector<std::string> result;
    for (const auto& entry : fs::directory_iterator(folder_path)) {
        if (!entry.is_regular_file()) {
            continue;
        }
        const auto filename = entry.path().filename().string();
        if (filename.size() >= search_string.size()
            && filename.substr(filename.size() - search_string.size()) == search_string) {
            result.push_back(filename);
        }
    }
    return result;
}

std::string FilePathLib::AddParentPath(const std::string& filename, const std::string& parent_path)
{
    if (filename.empty()) {
        return {};
    }

    const fs::path input(filename);
    if (input.is_absolute()) {
        return filename;
    }

    return (fs::path(parent_path) / input).lexically_normal().string();
}



    std::filesystem::path FilePathLib::DebugFilePath(const std::string& prefix, const std::string& suffix)
    {
        std::filesystem::path folder(automationtest::utilities::GlobalSetting::DebugViewImageFileFolder());
        if (folder.empty()) {
            folder = std::filesystem::current_path();
        }
        return folder / (prefix + "_" + automationtest::utilities::IdGenerator::IdWithDateTime() + suffix);
    }

    std::filesystem::path FilePathLib::DebugImageFilePath(const std::string& prefix)
    {
        std::filesystem::path folder(automationtest::utilities::GlobalSetting::DebugViewImageFileFolder());
        if (folder.empty()) {
            folder = std::filesystem::current_path();
        }
        return DebugFilePath(prefix, ".bmp");
    }

    std::filesystem::path FilePathLib::DebugJsonFilePath(const std::string& prefix)
    {
        std::filesystem::path folder(automationtest::utilities::GlobalSetting::DebugViewImageFileFolder());
        if (folder.empty()) {
            folder = std::filesystem::current_path();
        }
        return DebugFilePath(prefix, ".json");
    }



} // namespace automationtest::utilities
