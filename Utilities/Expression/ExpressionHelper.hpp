#ifndef AUTOMATIOTEST_EXPRESSION_EXPRESSIONHELPER_HPP
#define AUTOMATIOTEST_EXPRESSION_EXPRESSIONHELPER_HPP

#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace automationtest::utilities::expression {

class ExpressionHelper {
public:
    static std::string StringConcatenate(const std::vector<std::optional<std::string>>& values);
    static std::optional<std::string> SubString(const std::optional<std::string>& value, int start_index, int length = std::numeric_limits<int>::max());
    static std::optional<std::string> StringReplaceIgnoreCase(const std::optional<std::string>& value, const std::string& search_text, const std::string& replace_text = "");
    static double AddNumber(double d1, double d2, std::optional<double> d3 = std::nullopt) noexcept;
    static int AddInteger(int d1, int d2, std::optional<int> d3 = std::nullopt) noexcept;
    static double TimeNumber(double d1, double d2, std::optional<double> d3 = std::nullopt) noexcept;
};

} // namespace automationtest::utilities::expression

#endif // AUTOMATIOTEST_EXPRESSION_EXPRESSIONHELPER_HPP
