#ifndef AUTOMATIOTEST_TYPES_LINKEDHASHSET_HPP
#define AUTOMATIOTEST_TYPES_LINKEDHASHSET_HPP

#include <algorithm>
#include <optional>
#include <unordered_set>
#include <vector>

namespace automationtest::utilities::types {

template <typename T>
class LinkedHashSet {
public:
    [[nodiscard]] std::size_t Count() const noexcept { return list_.size(); }
    [[nodiscard]] bool Empty() const noexcept { return list_.empty(); }

    [[nodiscard]] const T& operator[](std::size_t index) const { return list_.at(index); }
    T& operator[](std::size_t index) { return list_.at(index); }

    bool Add(const T& item)
    {
        if (set_.insert(item).second) {
            list_.push_back(item);
            return true;
        }
        return false;
    }

    [[nodiscard]] bool Contains(const T& item) const
    {
        return set_.find(item) != set_.end();
    }

    void Clear()
    {
        set_.clear();
        list_.clear();
    }

    bool Remove(const T& item)
    {
        if (set_.erase(item) == 0) {
            return false;
        }

        auto it = std::find(list_.begin(), list_.end(), item);
        if (it != list_.end()) {
            list_.erase(it);
        }
        return true;
    }

    [[nodiscard]] std::optional<std::size_t> IndexOf(const T& item) const
    {
        auto it = std::find(list_.begin(), list_.end(), item);
        if (it == list_.end()) {
            return std::nullopt;
        }
        return static_cast<std::size_t>(std::distance(list_.begin(), it));
    }

    void Insert(std::size_t index, const T& item)
    {
        if (Contains(item)) {
            return;
        }

        list_.insert(list_.begin() + static_cast<std::ptrdiff_t>(index), item);
        set_.insert(item);
    }

    void RemoveAt(std::size_t index)
    {
        const T item = list_.at(index);
        list_.erase(list_.begin() + static_cast<std::ptrdiff_t>(index));
        set_.erase(item);
    }

    [[nodiscard]] const std::vector<T>& Items() const noexcept
    {
        return list_;
    }

private:
    std::vector<T> list_ {};
    std::unordered_set<T> set_ {};
};

} // namespace automationtest::utilities::types

#endif // AUTOMATIOTEST_TYPES_LINKEDHASHSET_HPP
