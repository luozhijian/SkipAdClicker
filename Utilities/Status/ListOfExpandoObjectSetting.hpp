#ifndef AUTOMATIOTEST_STATUS_LISTOFEXPANDOOBJECTSETTING_HPP
#define AUTOMATIOTEST_STATUS_LISTOFEXPANDOOBJECTSETTING_HPP

#include <string>
#include <unordered_map>
#include <vector>

namespace automationtest::utilities::status {

using DynamicRow = std::unordered_map<std::string, std::string>;

class ListOfExpandoObjectSetting {
public:
    void Add(DynamicRow item);
    void Clear();
    [[nodiscard]] bool Contains(const DynamicRow& item) const;
    [[nodiscard]] std::size_t Count() const noexcept;
    [[nodiscard]] const std::vector<DynamicRow>& Items() const noexcept;

private:
    std::vector<DynamicRow> items_ {};
};

} // namespace automationtest::utilities::status

#endif // AUTOMATIOTEST_STATUS_LISTOFEXPANDOOBJECTSETTING_HPP
