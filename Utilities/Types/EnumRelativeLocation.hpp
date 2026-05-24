#ifndef AUTOMATIOTEST_TYPES_ENUMRELATIVELOCATION_HPP
#define AUTOMATIOTEST_TYPES_ENUMRELATIVELOCATION_HPP

namespace automationtest::utilities::types {

enum class EnumRelativeLocation {
    None = 0,
    Left,
    Right,
    Above,
    Below,
    AboveRight,
    AboveLeft,
    BelowRight,
    BelowLeft,
    SameRowLeftClose,
    SameRowRightClose
};

} // namespace automationtest::utilities::types

#endif // AUTOMATIOTEST_TYPES_ENUMRELATIVELOCATION_HPP
