#ifndef AUTOMATIOTEST_EORIENTATION_HPP
#define AUTOMATIOTEST_EORIENTATION_HPP

namespace automationtest::utilities {

enum class eOrientation {
    Horizontal = 0,
    Vertical = 1
};

class eOrientationHelper {
public:
    static eOrientation Rotate(eOrientation orientation) noexcept;
};

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_EORIENTATION_HPP
