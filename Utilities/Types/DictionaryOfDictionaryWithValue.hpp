#ifndef AUTOMATIOTEST_TYPES_DICTIONARYOFDICTIONARYWITHVALUE_HPP
#define AUTOMATIOTEST_TYPES_DICTIONARYOFDICTIONARYWITHVALUE_HPP

#include <string>
#include <unordered_map>

namespace automationtest::utilities::types {

struct OneDictionaryWithValue {
    std::string value {};
    std::unordered_map<std::string, std::string> values {};
};

struct DictionaryOfDictionaryWithValue {
    std::unordered_map<std::string, OneDictionaryWithValue> values {};
};

} // namespace automationtest::utilities::types

#endif // AUTOMATIOTEST_TYPES_DICTIONARYOFDICTIONARYWITHVALUE_HPP
