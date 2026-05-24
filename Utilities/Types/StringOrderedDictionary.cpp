#include "StringOrderedDictionary.hpp"

#include <algorithm>
#include <cctype>

namespace automationtest::utilities::types {

void StringOrderedDictionary::Add(const std::string& key, const std::string& value)
{
    if (ContainsKey(key)) {
        throw std::invalid_argument("Key already exists: " + key);
    }
    items_.emplace_back(key, value);
}

bool StringOrderedDictionary::TryAdd(const std::string& key, const std::string& value)
{
    const auto index = FindIndex(key);
    if (index.has_value()) {
        items_[*index].second = value;
        return false;
    }
    items_.emplace_back(key, value);
    return true;
}

bool StringOrderedDictionary::ContainsKey(const std::string& key) const
{
    return FindIndex(key).has_value();
}

std::optional<std::string> StringOrderedDictionary::TryGetValue(const std::string& key) const
{
    const auto index = FindIndex(key);
    if (!index.has_value()) {
        return std::nullopt;
    }
    return items_[*index].second;
}

std::size_t StringOrderedDictionary::Count() const noexcept
{
    return items_.size();
}

std::vector<std::string> StringOrderedDictionary::Keys() const
{
    std::vector<std::string> result;
    result.reserve(items_.size());
    for (const auto& item : items_) {
        result.push_back(item.first);
    }
    return result;
}

std::vector<std::string> StringOrderedDictionary::Values() const
{
    std::vector<std::string> result;
    result.reserve(items_.size());
    for (const auto& item : items_) {
        result.push_back(item.second);
    }
    return result;
}

std::vector<std::pair<std::string, std::string>> StringOrderedDictionary::Items() const
{
    return items_;
}

void StringOrderedDictionary::Clear()
{
    items_.clear();
}

bool StringOrderedDictionary::Remove(const std::string& key)
{
    const auto index = FindIndex(key);
    if (!index.has_value()) {
        return false;
    }
    items_.erase(items_.begin() + static_cast<std::ptrdiff_t>(*index));
    return true;
}

const std::string& StringOrderedDictionary::operator[](const std::string& key) const
{
    const auto index = FindIndex(key);
    if (!index.has_value()) {
        throw std::out_of_range("Key not found: " + key);
    }
    return items_[*index].second;
}

std::string& StringOrderedDictionary::operator[](const std::string& key)
{
    const auto index = FindIndex(key);
    if (index.has_value()) {
        return items_[*index].second;
    }

    items_.emplace_back(key, std::string {});
    return items_.back().second;
}

bool StringOrderedDictionary::EqualsIgnoreCase(const std::string& left, const std::string& right)
{
    if (left.size() != right.size()) {
        return false;
    }

    for (std::size_t i = 0; i < left.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(left[i])) != std::tolower(static_cast<unsigned char>(right[i]))) {
            return false;
        }
    }
    return true;
}

std::optional<std::size_t> StringOrderedDictionary::FindIndex(const std::string& key) const
{
    for (std::size_t i = 0; i < items_.size(); ++i) {
        if (EqualsIgnoreCase(items_[i].first, key)) {
            return i;
        }
    }
    return std::nullopt;
}

} // namespace automationtest::utilities::types
