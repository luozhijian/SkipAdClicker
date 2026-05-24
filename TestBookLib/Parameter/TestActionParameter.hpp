#ifndef AUTOMATIOTEST_TESTBOOKLIB_PARAMETER_TESTACTIONPARAMETER_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_PARAMETER_TESTACTIONPARAMETER_HPP

#include <string>

namespace automationtest::testbooklib::parameter {

class TestActionParameter {
public:
    TestActionParameter() = default;
    explicit TestActionParameter(std::string parameter_value);

    [[nodiscard]] const std::string& Value() const noexcept;

private:
    std::string value_ {};
};

} // namespace automationtest::testbooklib::parameter

#endif // AUTOMATIOTEST_TESTBOOKLIB_PARAMETER_TESTACTIONPARAMETER_HPP
