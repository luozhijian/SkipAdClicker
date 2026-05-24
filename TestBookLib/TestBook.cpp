#include "TestBook.hpp"

namespace automationtest::testbooklib {

void TestBook::AddPage(std::unique_ptr<TestPage> page)
{
    if (start_page_ == nullptr) {
        start_page_ = page.get();
    }
    pages_.push_back(std::move(page));
}

TestPage* TestBook::StartPage() const noexcept
{
    return start_page_;
}

const std::vector<std::unique_ptr<TestPage>>& TestBook::Pages() const noexcept
{
    return pages_;
}

void TestBook::ForceLoad()
{
}

} // namespace automationtest::testbooklib
