#ifndef AUTOMATIOTEST_ATTRIBUTE_TESTBOOKACTIONCLASSATTRIBUTE_HPP
#define AUTOMATIOTEST_ATTRIBUTE_TESTBOOKACTIONCLASSATTRIBUTE_HPP

#include "../Reflection/Reflection.hpp"

namespace automationtest::utilities::attribute {

struct TestBookActionClassAttribute {
};

[[nodiscard]] constexpr auto ReflectTestBookActionClassAttribute() noexcept
{
    return ::automationtest::utilities::reflection::Reflect<TestBookActionClassAttribute>();
}

} // namespace automationtest::utilities::attribute

AUTOMATIONTEST_REFLECT_EMPTY_TYPE(::automationtest::utilities::attribute::TestBookActionClassAttribute)

#endif // AUTOMATIOTEST_ATTRIBUTE_TESTBOOKACTIONCLASSATTRIBUTE_HPP
