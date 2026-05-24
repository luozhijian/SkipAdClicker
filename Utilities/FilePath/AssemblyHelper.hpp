#ifndef AUTOMATIOTEST_FILEPATH_ASSEMBLYHELPER_HPP
#define AUTOMATIOTEST_FILEPATH_ASSEMBLYHELPER_HPP

#include <string>
#include <unordered_set>

namespace automationtest::utilities::file_path {

class AssemblyHelper {
public:
    explicit AssemblyHelper(const std::string& file_path = {});
    [[nodiscard]] bool ShouldIgnoreThisDll(const std::string& assembly_name) const;

private:
    std::unordered_set<std::string> public_token_cache_ {};
    std::unordered_set<std::string> name_cache_ {};
};

} // namespace automationtest::utilities::file_path

#endif // AUTOMATIOTEST_FILEPATH_ASSEMBLYHELPER_HPP
