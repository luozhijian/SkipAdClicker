#include "LinqExtension.hpp"

#include <map>
#include <stdexcept>

namespace automationtest::utilities {

std::vector<std::pair<int, std::vector<int>>> LinqExtension::GroupByNear(const std::vector<int>& values, int max_gap)
{
    if (values.empty()) {
        return {};
    }
    auto sorted = values;
    std::sort(sorted.begin(), sorted.end());
    std::vector<std::pair<int, std::vector<int>>> result;
    std::vector<int> current {sorted.front()};
    int previous = sorted.front();
    for (std::size_t i = 1; i < sorted.size(); ++i) {
        if (std::abs(previous - sorted[i]) <= max_gap) {
            current.push_back(sorted[i]);
        } else {
            result.emplace_back(previous, current);
            current = {sorted[i]};
        }
        previous = sorted[i];
    }
    result.emplace_back(previous, current);
    return result;
}

types::IntPair LinqExtension::FindNearestTwo(const std::vector<int>& values, int value)
{
    if (values.size() < 2) {
        throw std::runtime_error("Not enough values");
    }
    int last = values.front();
    for (std::size_t i = 1; i < values.size(); ++i) {
        if (last < value && values[i] > value) {
            return types::IntPair(last, values[i]);
        }
        last = values[i];
    }
    throw std::runtime_error("Cannot find nearest values");
}

std::vector<types::IntPair> LinqExtension::PairWithNext(const std::vector<int>& values)
{
    std::vector<types::IntPair> result;
    for (std::size_t i = 1; i < values.size(); ++i) {
        result.emplace_back(values[i - 1], values[i]);
    }
    return result;
}

std::vector<std::uint8_t> LinqExtension::FindMaxCountColor(const std::vector<std::byte>& values, int width, int height, int stride)
{
    std::vector<std::uint8_t> result;
    for (int row = 0; row < height; ++row) {
        std::map<std::uint8_t, int> counts;
        const int start = row * stride;
        for (int column = 0; column < width; ++column) {
            ++counts[static_cast<std::uint8_t>(values[start + column])];
        }
        const auto best = std::max_element(counts.begin(), counts.end(), [](const auto& left, const auto& right) {
            return left.second < right.second;
        });
        result.push_back(best == counts.end() ? 0 : best->first);
    }
    return result;
}

std::vector<int> LinqExtension::FindCountNotEqualToColorByRow(const std::vector<std::byte>& values, int width, int height, int stride, std::uint8_t color)
{
    std::vector<int> result;
    for (int row = 0; row < height; ++row) {
        int count = 0;
        const int start = row * stride;
        for (int column = 0; column < width; ++column) {
            if (static_cast<std::uint8_t>(values[start + column]) != color) {
                ++count;
            }
        }
        result.push_back(count);
    }
    return result;
}

std::vector<int> LinqExtension::FindCountNotEqualToColorByColumn(const std::vector<std::byte>& values, int width, int height, int stride, std::uint8_t color)
{
    std::vector<int> result;
    for (int column = 0; column < width; ++column) {
        int count = 0;
        int start = column;
        for (int row = 0; row < height; ++row, start += stride) {
            const int diff = static_cast<int>(static_cast<std::uint8_t>(values[start])) - static_cast<int>(color);
            if (diff > 3 || diff < -3) {
                ++count;
            }
        }
        result.push_back(count);
    }
    return result;
}

} // namespace automationtest::utilities
