#ifndef AUTOMATIOTEST_FILEPATHLIB_HPP
#define AUTOMATIOTEST_FILEPATHLIB_HPP

#include <string>
#include <vector>
#include <filesystem>

namespace automationtest::utilities {

class FilePathLib {
public:
    static void CreateFolderIfNotExists(const std::string& path);
    static std::string PathCombine(const std::string& path1, const std::string& path2, const std::string& path3 = {}, const std::string& path4 = {});
    static void CopyFileWithName(const std::string& source, const std::string& destination, bool overwrite = true);
    static int CopyFilesToAnotherFolder(const std::string& source_folder, const std::string& destination_folder, const std::string& pattern, bool overwrite = true);
    static bool CanRemoveFileSuccessfully(const std::string& destination_file);
    static std::string CopyFileToFolder(const std::string& source, const std::string& destination_path, bool overwrite = true);
    static int RemoveAllFileInFolder(const std::string& destination_path, const std::string& pattern = "*", int max_count_of_file_to_remove = 2);
    static int TryRemoveAllFileInFolder(const std::string& destination_path, const std::string& pattern = "*", int max_count_of_file_to_remove = 2);
    static bool CheckIfFileExists(const std::string& source);
    static std::string GenerateTestingOutputFilename(const std::string& filename, const std::string& function_name, const std::string& extension = ".png");
    static std::string AddCountToPathName(const std::string& filename);
    static std::string GenerateOutputResult(const std::string& filename, const std::string& prefix);
    static void MoveFilesWithPrefix(const std::string& source_folder, const std::string& destination_folder, const std::string& prefix);
    static bool SafeSaveToFile(const std::string& file_path, const std::string& content);
    static void SaveToFile(const std::string& file_path, const std::string& content);
    static std::vector<std::string> FindFilesEndingWith(const std::string& folder_path, const std::string& search_string);
    static std::string AddParentPath(const std::string& filename, const std::string& parent_path);


    static std::filesystem::path DebugFilePath(const std::string& prefix, const std::string& suffix);
    static std::filesystem::path DebugImageFilePath(const std::string& prefix);
    static std::filesystem::path DebugJsonFilePath(const std::string& prefix);


};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_FILEPATHLIB_HPP
