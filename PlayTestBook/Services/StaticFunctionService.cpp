#include "StaticFunctionService.hpp"

#include "RecentActivityService.hpp"

#include "../../Utilities/Common.hpp"
#include "../../Utilities/DependencyInjection/DependencyStore.hpp"

namespace automationtest::playtestbook::services {

StaticFunctionService& StaticFunctionService::Instance()
{
    static StaticFunctionService instance;
    return instance;
}

void StaticFunctionService::RegisterDefaultFunctions()
{
    if (default_functions_registered_) {
        return;
    }

    auto& load_functions = utilities::status::LoadFunctions::Instance();
    utilities::RegisterBindings(load_functions);
    RecentActivityService::Instance().RegisterBindings(load_functions);
    utilities::dependency_injection::DependencyStore::Instance().AddType("RecentActivityService", &RecentActivityService::Instance());
    default_functions_registered_ = true;
}

std::shared_ptr<testbooklib::TestBook> StaticFunctionService::LoadFunctions(const std::string& folder)
{
    RegisterDefaultFunctions();
    auto book = std::make_shared<testbooklib::TestBook>(testbooklib::parser::TestBookParser {}.Parse(folder));
    loaded_books_.push_back(book);
    return book;
}

const std::vector<std::shared_ptr<testbooklib::TestBook>>& StaticFunctionService::LoadedBooks() const noexcept
{
    return loaded_books_;
}

} // namespace automationtest::playtestbook::services
