#ifndef AUTOMATIOTEST_DICTIONARYLIB_HPP
#define AUTOMATIOTEST_DICTIONARYLIB_HPP

#include "Types/StringOrderedDictionary.hpp"

#include <string>
#include <unordered_map>

namespace automationtest::utilities {

class DictionaryLib {
public:
    static std::string DictionaryGetValue(const std::unordered_map<std::string, std::string>& dict, const std::string& key);
    static std::string DictionaryTryGetValue(const std::unordered_map<std::string, std::string>& dict, const std::string& key);
    static std::string DictionaryTryGetValueAndRemove(std::unordered_map<std::string, std::string>& dict, const std::string& key);
    static std::unordered_map<std::string, std::string> JoinTwoStringDictionary(
        const std::unordered_map<std::string, std::string>& dict1,
        const std::unordered_map<std::string, std::string>& dict2,
        bool join_by_key_in_first = true,
        bool match_use_key_in_second = true,
        bool result_key_from_second = true);
    static types::StringOrderedDictionary ReadOneStringWithSeperatorsAsDictionary(
        const std::string& text,
        const std::string& separator_between_item,
        const std::string& separator_in_item,
        bool first_as_key = true);
    static std::unordered_map<std::string, std::string> UnionTwoStringDictionary(
        const std::unordered_map<std::string, std::string>& dict1,
        const std::unordered_map<std::string, std::string>& dict2);
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_DICTIONARYLIB_HPP
