#ifndef AUTOMATIOTEST_TESTBOOKLIB_ACTION_ACTIONFACTORY_HPP
#define AUTOMATIOTEST_TESTBOOKLIB_ACTION_ACTIONFACTORY_HPP

#include "BlockAction.hpp"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace automationtest::testbooklib::action {

using BlockActionCreator = std::function<std::unique_ptr<BlockAction>(int, const std::string&)>;

class ActionFactory {
public:
    static void RegisterBlockAction(const std::string& name, BlockActionCreator creator);
    static std::unique_ptr<TestAction> CreateAction(const std::vector<std::string>& parts, const std::string& whole_line, int line_number);

private:
    static std::unordered_map<std::string, BlockActionCreator>& Registry();
};

} // namespace automationtest::testbooklib::action

#endif // AUTOMATIOTEST_TESTBOOKLIB_ACTION_ACTIONFACTORY_HPP
