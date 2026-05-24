#ifndef AUTOMATIOTEST_FILEHELPER_HPP
#define AUTOMATIOTEST_FILEHELPER_HPP

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace automationtest::utilities {

class FileHelper {
public:
    static std::unordered_map<std::string, std::string> ReadFileToDictionary(const std::string& file_path, const std::vector<char>& additional_trim = {});
    static std::string FileReadAllText(const std::string& filename);
    static std::string FileReadAllTextWithLnk(const std::string& filename);
    static std::optional<std::string> GetShortcutTarget(const std::string& shortcut_path);
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_FILEHELPER_HPP
