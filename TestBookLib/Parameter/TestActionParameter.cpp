#include "TestActionParameter.hpp"

namespace automationtest::testbooklib::parameter {

TestActionParameter::TestActionParameter(std::string parameter_value)
    : value_(std::move(parameter_value))
{
}

const std::string& TestActionParameter::Value() const noexcept
{
    return value_;
}

} // namespace automationtest::testbooklib::parameter
