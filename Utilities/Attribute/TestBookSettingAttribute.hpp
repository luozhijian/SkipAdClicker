#ifndef AUTOMATIOTEST_ATTRIBUTE_TESTBOOKSETTINGATTRIBUTE_HPP
#define AUTOMATIOTEST_ATTRIBUTE_TESTBOOKSETTINGATTRIBUTE_HPP

#include "../Reflection/Reflection.hpp"

#include <string>
#include <utility>

namespace automationtest::utilities::attribute {

struct TestBookSettingAttribute {
    std::string setting_name {};

    TestBookSettingAttribute() = default;
    explicit TestBookSettingAttribute(std::string setting_name_value)
        : setting_name(std::move(setting_name_value))
    {
    }
};

[[nodiscard]] constexpr auto ReflectTestBookSettingAttribute() noexcept
{
    return ::automationtest::utilities::reflection::Reflect<TestBookSettingAttribute>();
}

} // namespace automationtest::utilities::attribute

AUTOMATIONTEST_REFLECT_TYPE(::automationtest::utilities::attribute::TestBookSettingAttribute, "setting_name")

#endif // AUTOMATIOTEST_ATTRIBUTE_TESTBOOKSETTINGATTRIBUTE_HPP
