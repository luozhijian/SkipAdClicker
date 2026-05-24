#ifndef AUTOMATIOTEST_MATHLIB_HPP
#define AUTOMATIOTEST_MATHLIB_HPP

#include <algorithm>
#include <optional>
#include <stdexcept>
#include <vector>

namespace automationtest::utilities {

class MathLib {
public:
    struct ConcentratedGroup {
        int start {0};
        int end {0};
        double weighted_avg {0.0};
        int total_count {0};
    };

    static constexpr double Epsilon = 0.009;
    static constexpr double EpsilonNegative = -0.009;

    static std::pair<int, int> MinMax(int first, int second) noexcept;
    static bool IsEqual(std::optional<double> first, std::optional<double> second) noexcept;
    static bool Between(double value, double minimum, double maximum) noexcept;
    static bool Between(int value, double minimum, double maximum) noexcept;
    static bool Between(int value, int minimum, int maximum) noexcept;
    static bool Between(short value, int minimum, int maximum) noexcept;
    static ConcentratedGroup GetMostConcentratedGroup(const std::vector<int>& histogram);

    template <typename T>
    static T MinValue(const std::vector<T>& values)
    {
        if (values.empty()) {
            throw std::invalid_argument("At least one value must be provided.");
        }

        return *std::min_element(values.begin(), values.end());
    }
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_MATHLIB_HPP
