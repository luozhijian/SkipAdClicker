#ifndef AUTOMATIOTEST_EXTENSION_HPP
#define AUTOMATIOTEST_EXTENSION_HPP

#include <algorithm>
#include <initializer_list>

namespace automationtest::utilities {

template <typename T>
bool In(const T& value, std::initializer_list<T> values)
{
    return std::find(values.begin(), values.end(), value) != values.end();
}

} // namespace automationtest::utilities

#endif // AUTOMATIOTEST_EXTENSION_HPP
