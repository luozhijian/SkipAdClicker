#ifndef AUTOMATIOTEST_LINQEXTENSION_HPP
#define AUTOMATIOTEST_LINQEXTENSION_HPP

#include "Types/IntPair.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <unordered_set>
#include <utility>
#include <vector>

namespace automationtest::utilities {

class LinqExtension {
public:
    template <typename TSource, typename TKey>
    static std::unordered_set<TKey> ToHashSet(const std::vector<TSource>& source, const std::function<TKey(const TSource&)>& selector)
    {
        std::unordered_set<TKey> result;
        for (const auto& value : source) {
            result.insert(selector(value));
        }
        return result;
    }

    template <typename TSource, typename TResult>
    static std::vector<TResult> SelectWithPrevious(const std::vector<TSource>& source, const std::function<TResult(const TSource&, const TSource&)>& projection)
    {
        std::vector<TResult> result;
        for (std::size_t i = 1; i < source.size(); ++i) {
            result.push_back(projection(source[i - 1], source[i]));
        }
        return result;
    }

    static std::vector<std::pair<int, std::vector<int>>> GroupByNear(const std::vector<int>& values, int max_gap);
    static types::IntPair FindNearestTwo(const std::vector<int>& values, int value);
    static std::vector<types::IntPair> PairWithNext(const std::vector<int>& values);
    static std::vector<std::uint8_t> FindMaxCountColor(const std::vector<std::byte>& values, int width, int height, int stride);
    static std::vector<int> FindCountNotEqualToColorByRow(const std::vector<std::byte>& values, int width, int height, int stride, std::uint8_t color);
    static std::vector<int> FindCountNotEqualToColorByColumn(const std::vector<std::byte>& values, int width, int height, int stride, std::uint8_t color);
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_LINQEXTENSION_HPP
