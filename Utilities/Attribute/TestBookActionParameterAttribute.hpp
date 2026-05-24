#ifndef AUTOMATIOTEST_ATTRIBUTE_TESTBOOKACTIONPARAMETERATTRIBUTE_HPP
#define AUTOMATIOTEST_ATTRIBUTE_TESTBOOKACTIONPARAMETERATTRIBUTE_HPP

#include "../Reflection/Reflection.hpp"

#include <string>
#include <utility>

namespace automationtest::utilities::attribute {

struct TestBookActionParameterAttribute {
    std::string parameter_name {};

    TestBookActionParameterAttribute() = default;
    explicit TestBookActionParameterAttribute(std::string parameter_name_value)
        : parameter_name(std::move(parameter_name_value))
    {
    }
};

[[nodiscard]] constexpr auto ReflectTestBookActionParameterAttribute() noexcept
{
    return ::automationtest::utilities::reflection::Reflect<TestBookActionParameterAttribute>();
}

} // namespace automationtest::utilities::attribute

AUTOMATIONTEST_REFLECT_TYPE(::automationtest::utilities::attribute::TestBookActionParameterAttribute, "parameter_name")

#endif // AUTOMATIOTEST_ATTRIBUTE_TESTBOOKACTIONPARAMETERATTRIBUTE_HPP
