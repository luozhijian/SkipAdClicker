#ifndef AUTOMATIOTEST_TESTBOOKLIB_TESTBOOK_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_TESTBOOK_HPP

#include "TestPage.hpp"

#include <memory>
#include <vector>

namespace automationtest::testbooklib {

class TestBook {
public:
    void AddPage(std::unique_ptr<TestPage> page);
    [[nodiscard]] TestPage* StartPage() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<TestPage>>& Pages() const noexcept;
    static void ForceLoad();

private:
    TestPage* start_page_ {nullptr};
    std::vector<std::unique_ptr<TestPage>> pages_ {};
};

} // namespace automationtest::testbooklib

#endif // AUTOMATIOTEST_TESTBOOKLIB_TESTBOOK_HPP
