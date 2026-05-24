#include "ScreenLayoutHelper.hpp"

namespace automationtest::userinterfacelib {

Rectangle ScreenLayoutHelper::LayoutGetPart(const ScreenLayout& layout, const std::string& part_name)
{
    const auto iterator = layout.layouts.find(part_name);
    if (iterator == layout.layouts.end()) {
        throw std::runtime_error("Cannot find part " + part_name);
    }

    return iterator->second;
}

} // namespace automationtest::userinterfacelib
