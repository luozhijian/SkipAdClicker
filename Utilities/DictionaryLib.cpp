#include "DictionaryLib.hpp"

#include <sstream>
#include <stdexcept>

namespace automationtest::utilities {

std::string DictionaryLib::DictionaryGetValue(const std::unordered_map<std::string, std::string>& dict, const std::string& key)
{
    const auto it = dict.find(key);
    if (it == dict.end()) {
        throw std::runtime_error(key + " is not in dictionary");
    }
    return it->second;
}

std::string DictionaryLib::DictionaryTryGetValue(const std::unordered_map<std::string, std::string>& dict, const std::string& key)
{
    const auto it = dict.find(key);
    return it == dict.end() ? std::string {} : it->second;
}

std::string DictionaryLib::DictionaryTryGetValueAndRemove(std::unordered_map<std::string, std::string>& dict, const std::string& key)
{
    const auto it = dict.find(key);
    if (it == dict.end()) {
        return {};
    }
    const auto value = it->second;
    dict.erase(it);
    return value;
}

std::unordered_map<std::string, std::string> DictionaryLib::JoinTwoStringDictionary(
    const std::unordered_map<std::string, std::string>& dict1,
    const std::unordered_map<std::string, std::string>& dict2,
    bool join_by_key_in_first,
    bool match_use_key_in_second,
    bool result_key_from_second)
{
    std::unordered_map<std::string, std::string> merged;
    for (const auto& [key, value] : dict1) {
        const std::string join_key = join_by_key_in_first ? key : value;
        bool processed = false;

        if (match_use_key_in_second) {
            const auto it = dict2.find(join_key);
            if (it != dict2.end()) {
                const std::string new_key = result_key_from_second ? it->second : (join_by_key_in_first ? value : key);
                const std::string new_value = result_key_from_second ? (join_by_key_in_first ? value : key) : it->second;
                merged[new_key] = new_value;
                processed = true;
            }
        } else {
            for (const auto& [key2, value2] : dict2) {
                if (value2 == join_key) {
                    const std::string new_key = result_key_from_second ? key2 : (join_by_key_in_first ? value : key);
                    const std::string new_value = result_key_from_second ? (join_by_key_in_first ? value : key) : key2;
                    merged[new_key] = new_value;
                    processed = true;
                    break;
                }
            }
        }

        if (!processed) {
            merged[key] = value;
        }
    }
    return merged;
}

types::StringOrderedDictionary DictionaryLib::ReadOneStringWithSeperatorsAsDictionary(
    const std::string& text,
    const std::string& separator_between_item,
    const std::string& separator_in_item,
    bool first_as_key)
{
    types::StringOrderedDictionary result;
    if (text.empty()) {
        return result;
    }

    std::size_t start = 0;
    while (start <= text.size()) {
        const auto end = text.find(separator_between_item, start);
        const auto part = text.substr(start, end == std::string::npos ? std::string::npos : end - start);
        if (!part.empty()) {
            const auto split = part.find(separator_in_item);
            const std::string key = part.substr(0, split);
            const std::string value = split == std::string::npos ? std::string {} : part.substr(split + separator_in_item.size());
            if (first_as_key) {
                result[key] = value;
            } else if (!value.empty()) {
                result[value] = key;
            }
        }
        if (end == std::string::npos) {
            break;
        }
        start = end + separator_between_item.size();
    }
    return result;
}

std::unordered_map<std::string, std::string> DictionaryLib::UnionTwoStringDictionary(
    const std::unordered_map<std::string, std::string>& dict1,
    const std::unordered_map<std::string, std::string>& dict2)
{
    auto result = dict1;
    for (const auto& [key, value] : dict2) {
        result[key] = value;
    }
    return result;
}

} // namespace automationtest::utilities
