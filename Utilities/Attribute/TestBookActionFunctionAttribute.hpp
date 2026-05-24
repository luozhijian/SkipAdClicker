#ifndef AUTOMATIOTEST_ATTRIBUTE_TESTBOOKACTIONFUNCTIONATTRIBUTE_HPP
#define AUTOMATIOTEST_ATTRIBUTE_TESTBOOKACTIONFUNCTIONATTRIBUTE_HPP

#include "../Reflection/Reflection.hpp"

#include <string>
#include <utility>

namespace automationtest::utilities::attribute {

struct TestBookActionFunctionAttribute {
    std::string function_name {};

    TestBookActionFunctionAttribute() = default;
    explicit TestBookActionFunctionAttribute(std::string function_name_value)
        : function_name(std::move(function_name_value))
    {
    }
};

[[nodiscard]] constexpr auto ReflectTestBookActionFunctionAttribute() noexcept
{
    return ::automationtest::utilities::reflection::Reflect<TestBookActionFunctionAttribute>();
}

} // namespace automationtest::utilities::attribute

AUTOMATIONTEST_REFLECT_TYPE(::automationtest::utilities::attribute::TestBookActionFunctionAttribute, "function_name")

#endif // AUTOMATIOTEST_ATTRIBUTE_TESTBOOKACTIONFUNCTIONATTRIBUTE_HPP
