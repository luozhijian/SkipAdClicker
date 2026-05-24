#include "SettingSqlblock.hpp"

namespace automationtest::utilities::settings {

void SettingSqlblock::ResolveConnectionStrings()
{
    for (auto& definition : sqls) {
        const auto it = connection_strings.find(definition.connection_string);
        if (it != connection_strings.end()) {
            definition.connection_string = it->second;
        }
    }
}

} // namespace automationtest::utilities::settings
