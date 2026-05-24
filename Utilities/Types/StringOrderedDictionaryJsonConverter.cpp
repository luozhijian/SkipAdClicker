#include "StringOrderedDictionaryJsonConverter.hpp"

#include <sstream>

namespace automationtest::utilities::types {

std::string StringOrderedDictionaryJsonConverter::Write(const StringOrderedDictionary& dictionary)
{
    std::ostringstream builder;
    builder << '[';
    const auto items = dictionary.Items();
    for (std::size_t i = 0; i < items.size(); ++i) {
        if (i > 0) {
            builder << ',';
        }
        builder << "{\"Key\":\"" << items[i].first << "\",\"Value\":\"" << items[i].second << "\"}";
    }
    builder << ']';
    return builder.str();
}

} // namespace automationtest::utilities::types
