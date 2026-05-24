#ifndef AUTOMATIOTEST_ATTRIBUTE_TESTBOOKBLOCKACTIONATTRIBUTE_HPP
#define AUTOMATIOTEST_ATTRIBUTE_TESTBOOKBLOCKACTIONATTRIBUTE_HPP

#include "../Reflection/Reflection.hpp"

#include <string>
#include <utility>

namespace automationtest::utilities::attribute {

struct TestBookBlockActionAttribute {
    std::string block_action_name {};

    TestBookBlockActionAttribute() = default;
    explicit TestBookBlockActionAttribute(std::string block_action_name_value)
        : block_action_name(std::move(block_action_name_value))
    {
    }
};

[[nodiscard]] constexpr auto ReflectTestBookBlockActionAttribute() noexcept
{
    return ::automationtest::utilities::reflection::Reflect<TestBookBlockActionAttribute>();
}

} // namespace automationtest::utilities::attribute

AUTOMATIONTEST_REFLECT_TYPE(::automationtest::utilities::attribute::TestBookBlockActionAttribute, "block_action_name")

#endif // AUTOMATIOTEST_ATTRIBUTE_TESTBOOKBLOCKACTIONATTRIBUTE_HPP
