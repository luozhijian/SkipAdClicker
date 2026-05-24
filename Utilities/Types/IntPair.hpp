#ifndef AUTOMATIOTEST_TYPES_INTPAIR_HPP
#define AUTOMATIOTEST_TYPES_INTPAIR_HPP

#include <string>

namespace automationtest::utilities::types {

class IntPair {
public:
    IntPair() = default;
    IntPair(int first, int second);

    [[nodiscard]] int First() const noexcept;
    [[nodiscard]] int Second() const noexcept;
    [[nodiscard]] int Length() const noexcept;
    [[nodiscard]] std::string ToString() const;

    friend bool operator==(const IntPair& left, const IntPair& right) noexcept;

private:
    int first_ {0};
    int second_ {0};
};

} // namespace automationtest::utilities::types

#endif // AUTOMATIOTEST_TYPES_INTPAIR_HPP
