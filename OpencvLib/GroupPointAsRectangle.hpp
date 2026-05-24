#ifndef AUTOMATIOTEST_OPENCVLIB_GROUPPOINTASRECTANGLE_HPP
#define AUTOMATIOTEST_OPENCVLIB_GROUPPOINTASRECTANGLE_HPP

#include "../Utilities/CommonTypes.hpp"

#include <array>
#include <vector>

namespace automationtest::opencvlib {

class GroupPointAsRectangle {
public:
    void AddPoint(int x, int y);
    void CompletedAdding();
    [[nodiscard]] std::vector<automationtest::utilities::Rectangle> GetGroupedResult() const;

private:
    using RectangleGroup = std::array<int, 5>;

    static bool PointInRange(int x, int y, const RectangleGroup& ranges) noexcept;
    bool PointNearRange(int x, int y, const RectangleGroup& ranges, int distance) const noexcept;
    static void ExpandRange(int x, int y, RectangleGroup& ranges) noexcept;
    static void MergeTwoRangeIntoOne(RectangleGroup& ranges, const RectangleGroup& ranges2) noexcept;
    [[nodiscard]] RectangleGroup AddOneGroup(int x, int y) const noexcept;
    bool TwoGroupIsNear(const RectangleGroup& group, const RectangleGroup& group2) const noexcept;
    void CheckAndMergeRangeIfItIsNear();

    std::vector<RectangleGroup> rectangles_corners_ {};
    int max_distance_ {250};
};

} // namespace automationtest::opencvlib

#endif // AUTOMATIOTEST_OPENCVLIB_GROUPPOINTASRECTANGLE_HPP
