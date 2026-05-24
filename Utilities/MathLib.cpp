#include "MathLib.hpp"

namespace automationtest::utilities {

std::pair<int, int> MathLib::MinMax(int first, int second) noexcept
{
    return {std::min(first, second), std::max(first, second)};
}

bool MathLib::IsEqual(std::optional<double> first, std::optional<double> second) noexcept
{
    if (first.has_value() && second.has_value()) {
        const double difference = *first - *second;
        return difference <= Epsilon && difference >= EpsilonNegative;
    }

    return first == second;
}

bool MathLib::Between(double value, double minimum, double maximum) noexcept
{
    return value > minimum && value < maximum;
}

bool MathLib::Between(int value, double minimum, double maximum) noexcept
{
    return value >= minimum && value <= maximum;
}

bool MathLib::Between(int value, int minimum, int maximum) noexcept
{
    return value >= minimum && value <= maximum;
}

bool MathLib::Between(short value, int minimum, int maximum) noexcept
{
    return value >= minimum && value <= maximum;
}

MathLib::ConcentratedGroup MathLib::GetMostConcentratedGroup(const std::vector<int>& histogram)
{
    if (histogram.empty()) {
        throw std::invalid_argument("histogram must contain at least one value.");
    }

    const int count = static_cast<int>(histogram.size());
    ConcentratedGroup best {};
    int index = 0;
    while (index < count) {
        while (index < count - 1 && histogram[index] == 0 && histogram[index + 1] == 0) {
            ++index;
        }

        if (index >= count) {
            break;
        }

        const int start = index;
        int total_count = 0;
        long long weighted_sum = 0;
        while (index < count) {
            if (index < count - 1 && histogram[index] == 0 && histogram[index + 1] == 0) {
                break;
            }

            total_count += histogram[index];
            weighted_sum += static_cast<long long>(index) * histogram[index];
            ++index;
        }

        if (total_count > 0 && total_count > best.total_count) {
            best = ConcentratedGroup {
                start,
                index - 1,
                static_cast<double>(weighted_sum) / total_count,
                total_count
            };
        }
    }

    return best;
}

} // namespace automationtest::utilities
