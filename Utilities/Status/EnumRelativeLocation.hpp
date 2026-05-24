#ifndef AUTOMATIOTEST_STATUS_ENUMRELATIVELOCATION_HPP
#define AUTOMATIOTEST_STATUS_ENUMRELATIVELOCATION_HPP

namespace automationtest::utilities::status {

enum class EnumRelativeLocation {
    LeftOf,
    RightOf,
    Above,
    Below,
    Near,
    Aligned,
    Overlaps,
    Inside,
    Contains,
    Touches,
    ZAbove,
    ZBelow,
    ImmediateBelow,
    ImmediateAbove,
    ImmediateRight,
    ImmediateLeft,
    SameRow,
    SameColumn,
    OrderBefore,
    OrderAfter,
    InGroup,
    LabelFor,
    DescribedBy,
    PlaceholderFor,
    Decorates,
    HasIcon,
    SameTextAs,
    SimilarText
};

} // namespace automationtest::utilities::status

#endif // AUTOMATIOTEST_STATUS_ENUMRELATIVELOCATION_HPP
