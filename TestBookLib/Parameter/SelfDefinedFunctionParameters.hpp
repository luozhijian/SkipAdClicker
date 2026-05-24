#ifndef AUTOMATIOTEST_TESTBOOKLIB_PARAMETER_SELFDEFINEDFUNCTIONPARAMETERS_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_PARAMETER_SELFDEFINEDFUNCTIONPARAMETERS_HPP

#include <any>
#include <string>
#include <unordered_map>
#include <vector>

namespace automationtest::testbooklib::parameter {

class SelfDefinedFunctionParameters {
public:
    [[nodiscard]] const std::vector<std::string>& GetParameterNames() const noexcept;
    void InitialAddParameter(const std::string& name);
    bool TryGetValue(const std::string& name, std::any& value) const;
    void AddValue(const std::string& name, std::any value);

private:
    static std::string NormalizeName(const std::string& name);

    std::vector<std::string> parameter_names_ {};
    std::unordered_map<std::string, std::any> parameters_ {};
};

} // namespace automationtest::testbooklib::parameter

#endif // AUTOMATIOTEST_TESTBOOKLIB_PARAMETER_SELFDEFINEDFUNCTIONPARAMETERS_HPP
