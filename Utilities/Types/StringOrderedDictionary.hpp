#ifndef AUTOMATIOTEST_TYPES_STRINGORDEREDDICTIONARY_HPP
#define AUTOMATIOTEST_TYPES_STRINGORDEREDDICTIONARY_HPP

#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace automationtest::utilities::types {

class StringOrderedDictionary {
public:
    void Add(const std::string& key, const std::string& value);
    [[nodiscard]] bool TryAdd(const std::string& key, const std::string& value);
    [[nodiscard]] bool ContainsKey(const std::string& key) const;
    [[nodiscard]] std::optional<std::string> TryGetValue(const std::string& key) const;
    [[nodiscard]] std::size_t Count() const noexcept;
    [[nodiscard]] std::vector<std::string> Keys() const;
    [[nodiscard]] std::vector<std::string> Values() const;
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> Items() const;
    void Clear();
    bool Remove(const std::string& key);
    const std::string& operator[](const std::string& key) const;
    std::string& operator[](const std::string& key);

private:
    static bool EqualsIgnoreCase(const std::string& left, const std::string& right);
    [[nodiscard]] std::optional<std::size_t> FindIndex(const std::string& key) const;

    std::vector<std::pair<std::string, std::string>> items_ {};
};

} // namespace automationtest::utilities::types

#endif // AUTOMATIOTEST_TYPES_STRINGORDEREDDICTIONARY_HPP
