#ifndef AUTOMATIOTEST_PLAYTESTBOOK_SERVICES_STATICFUNCTIONSERVICE_HPP
#define AUTOMATIOTEST_PLAYTESTBOOK_SERVICES_STATICFUNCTIONSERVICE_HPP

#include "../../TestBookLib/Parser/TestBookParser.hpp"

#include <memory>
#include <vector>

namespace automationtest::playtestbook::services {

class StaticFunctionService {
public:
    static StaticFunctionService& Instance();

    void RegisterDefaultFunctions();
    std::shared_ptr<testbooklib::TestBook> LoadFunctions(const std::string& folder);
    [[nodiscard]] const std::vector<std::shared_ptr<testbooklib::TestBook>>& LoadedBooks() const noexcept;

private:
    StaticFunctionService() = default;

    std::vector<std::shared_ptr<testbooklib::TestBook>> loaded_books_ {};
    bool default_functions_registered_ {false};
};

} // namespace automationtest::playtestbook::services

#endif // AUTOMATIOTEST_PLAYTESTBOOK_SERVICES_STATICFUNCTIONSERVICE_HPP
