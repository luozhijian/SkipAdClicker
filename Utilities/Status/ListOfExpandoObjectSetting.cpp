#include "ListOfExpandoObjectSetting.hpp"

#include <algorithm>
#include <utility>

namespace automationtest::utilities::status {

void ListOfExpandoObjectSetting::Add(DynamicRow item)
{
    items_.push_back(std::move(item));
}

void ListOfExpandoObjectSetting::Clear()
{
    items_.clear();
}

bool ListOfExpandoObjectSetting::Contains(const DynamicRow& item) const
{
    return std::find(items_.begin(), items_.end(), item) != items_.end();
}

std::size_t ListOfExpandoObjectSetting::Count() const noexcept
{
    return items_.size();
}

const std::vector<DynamicRow>& ListOfExpandoObjectSetting::Items() const noexcept
{
    return items_;
}

} // namespace automationtest::utilities::status
