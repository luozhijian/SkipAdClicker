#include "FileHelper.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace automationtest::utilities {

namespace {

std::string TrimCopy(std::string value, const std::vector<char>& extra_trim)
{
    auto should_trim = [&extra_trim](char ch) {
        return std::isspace(static_cast<unsigned char>(ch)) != 0
            || std::find(extra_trim.begin(), extra_trim.end(), ch) != extra_trim.end();
    };

    while (!value.empty() && should_trim(value.front())) {
        value.erase(value.begin());
    }
    while (!value.empty() && should_trim(value.back())) {
        value.pop_back();
    }
    return value;
}

}

std::unordered_map<std::string, std::string> FileHelper::ReadFileToDictionary(const std::string& file_path, const std::vector<char>& additional_trim)
{
    std::unordered_map<std::string, std::string> dictionary;
    std::ifstream input(file_path);
    std::string line;

    while (std::getline(input, line)) {
        const auto separator = line.find('=');
        if (separator == std::string::npos) {
            continue;
        }

        const auto key = TrimCopy(line.substr(0, separator), {});
        const auto value = TrimCopy(line.substr(separator + 1), additional_trim);
        dictionary[key] = value;
    }

    return dictionary;
}

std::string FileHelper::FileReadAllText(const std::string& filename)
{
    std::ifstream input(filename, std::ios::binary);
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

std::string FileHelper::FileReadAllTextWithLnk(const std::string& filename)
{
    const auto target = GetShortcutTarget(filename);
    return FileReadAllText(target.value_or(filename));
}

std::optional<std::string> FileHelper::GetShortcutTarget(const std::string& shortcut_path)
{
    if (shortcut_path.size() >= 4 && shortcut_path.substr(shortcut_path.size() - 4) == ".lnk") {
        return std::nullopt;
    }
    return shortcut_path;
}

} // namespace automationtest::utilities
