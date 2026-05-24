#include "AssemblyHelper.hpp"

#include "ReadFileAsValuePairList.hpp"

#include <filesystem>

namespace automationtest::utilities::file_path {

AssemblyHelper::AssemblyHelper(const std::string& file_path)
{
    if (file_path.empty()) {
        return;
    }

    const auto filename = std::filesystem::path(file_path) / "_IgnorePublicKeyOrDllName.txt";
    if (!std::filesystem::exists(filename)) {
        return;
    }

    const auto values = ReadFileAsValuePairList::Read(filename.string(), ":");
    for (const auto& [key, value] : values) {
        if (key == "PublicToken") {
            public_token_cache_.insert(value);
        } else if (key == "Name") {
            name_cache_.insert(value);
        }
    }
}

bool AssemblyHelper::ShouldIgnoreThisDll(const std::string& assembly_name) const
{
    for (const auto& name : name_cache_) {
        if (assembly_name.find(name) != std::string::npos) {
            return true;
        }
    }
    return public_token_cache_.find(assembly_name) != public_token_cache_.end();
}

} // namespace automationtest::utilities::file_path
