#include "IntPair.hpp"

#include <sstream>

namespace automationtest::utilities::types {

IntPair::IntPair(int first, int second)
    : first_(first)
    , second_(second)
{
}

int IntPair::First() const noexcept
{
    return first_;
}

int IntPair::Second() const noexcept
{
    return second_;
}

int IntPair::Length() const noexcept
{
    return second_ - first_;
}

std::string IntPair::ToString() const
{
    std::ostringstream builder;
    builder << first_ << ',' << second_ << ',' << Length();
    return builder.str();
}

bool operator==(const IntPair& left, const IntPair& right) noexcept
{
    return (left.first_ == right.first_ && left.second_ == right.second_)
        || (left.first_ == right.second_ && left.second_ == right.first_);
}

} // namespace automationtest::utilities::types
