#ifndef AUTOMATIOTEST_TYPES_STRINGORDEREDDICTIONARYJSONCONVERTER_HPP
#define AUTOMATIOTEST_TYPES_STRINGORDEREDDICTIONARYJSONCONVERTER_HPP

#include "StringOrderedDictionary.hpp"

#include <string>

namespace automationtest::utilities::types {

class StringOrderedDictionaryJsonConverter {
public:
    static std::string Write(const StringOrderedDictionary& dictionary);
};

} // namespace automationtest::utilities::types

#endif // AUTOMATIOTEST_TYPES_STRINGORDEREDDICTIONARYJSONCONVERTER_HPP
