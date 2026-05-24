#include "ActionFactory.hpp"

#include "../TestBookLibCommon.hpp"
#include "CheckVariableExists.hpp"
#include "CheckVariableIsEmptyOrWhiteSpace.hpp"
#include "OnErrorResumeNextAction.hpp"
#include "OnErrorSkipRestInBlockAction.hpp"
#include "RetryWhenException.hpp"
#include "ScrollbarClickWhenException.hpp"
#include "../FlowControls/DoWhileLoop.hpp"
#include "../FlowControls/DefFunction.hpp"
#include "../FlowControls/DictionaryLoop.hpp"
#include "../FlowControls/ForFilesInFolder.hpp"
#include "../FlowControls/ForLinesInFile.hpp"
#include "../FlowControls/ForLoop.hpp"
#include "../FlowControls/IfCondition.hpp"
#include "../FlowControls/IEnumerableLoop.hpp"
#include "../FlowControls/ForSubFoldersInFolder.hpp"
#include "../FlowControls/WhileLoop.hpp"
#include "../../Utilities/Exceptions/TestException.hpp"

namespace automationtest::testbooklib::action {

using automationtest::utilities::exceptions::TestException;

namespace {

void EnsureBuiltinsRegistered()
{
    static const bool registered = [] {
        ActionFactory::RegisterBlockAction("IfCondition", [](int line_number, const std::string& text) {
            return std::make_unique<flowcontrols::IfCondition>(line_number, text);
        });
        ActionFactory::RegisterBlockAction("ForLoop", [](int line_number, const std::string& text) {
            return std::make_unique<flowcontrols::ForLoop>(line_number, text);
        });
        ActionFactory::RegisterBlockAction("WhileLoop", [](int line_number, const std::string& text) {
            return std::make_unique<flowcontrols::WhileLoop>(line_number, text);
        });
        ActionFactory::RegisterBlockAction("DoWhileLoop", [](int line_number, const std::string& text) {
            return std::make_unique<flowcontrols::DoWhileLoop>(line_number, text);
        });
        ActionFactory::RegisterBlockAction("DefFunction", [](int line_number, const std::string& text) {
            return std::make_unique<flowcontrols::DefFunction>(line_number, text);
        });
        ActionFactory::RegisterBlockAction("def", [](int line_number, const std::string& text) {
            return std::make_unique<flowcontrols::DefFunction>(line_number, text);
        });
        ActionFactory::RegisterBlockAction("DictionaryLoop", [](int line_number, const std::string& text) {
            return std::make_unique<flowcontrols::DictionaryLoop>(line_number, text);
        });
        ActionFactory::RegisterBlockAction("RetryWhenException", [](int line_number, const std::string& text) {
            return std::make_unique<RetryWhenException>(line_number, text);
        });
        ActionFactory::RegisterBlockAction("CheckVariableExists", [](int line_number, const std::string& text) {
            return std::make_unique<CheckVariableExists>(line_number, text);
        });
        ActionFactory::RegisterBlockAction("CheckVariableIsEmptyOrWhiteSpace", [](int line_number, const std::string& text) {
            return std::make_unique<CheckVariableIsEmptyOrWhiteSpace>(line_number, text);
        });
        ActionFactory::RegisterBlockAction("OnErrorResumeNextAction", [](int line_number, const std::string& text) {
            return std::make_unique<OnErrorResumeNextAction>(line_number, text);
        });
        ActionFactory::RegisterBlockAction("OnErrorSkipRestInBlockAction", [](int line_number, const std::string& text) {
            return std::make_unique<OnErrorSkipRestInBlockAction>(line_number, text);
        });
        ActionFactory::RegisterBlockAction("ScrollbarClickWhenException", [](int line_number, const std::string& text) {
            return std::make_unique<ScrollbarClickWhenException>(line_number, text);
        });
        ActionFactory::RegisterBlockAction("ForLinesInFile", [](int line_number, const std::string& text) {
            return std::make_unique<flowcontrols::ForLinesInFile>(line_number, text);
        });
        ActionFactory::RegisterBlockAction("IEnumerableLoop", [](int line_number, const std::string& text) {
            return std::make_unique<flowcontrols::IEnumerableLoop>(line_number, text);
        });
        ActionFactory::RegisterBlockAction("ForFilesInFolder", [](int line_number, const std::string& text) {
            return std::make_unique<flowcontrols::ForFilesInFolder>(line_number, text);
        });
        ActionFactory::RegisterBlockAction("ForSubFoldersInFolder", [](int line_number, const std::string& text) {
            return std::make_unique<flowcontrols::ForSubFoldersInFolder>(line_number, text);
        });
        return true;
    }();

    (void)registered;
}

} // namespace

std::unordered_map<std::string, BlockActionCreator>& ActionFactory::Registry()
{
    static std::unordered_map<std::string, BlockActionCreator> registry {};
    return registry;
}

void ActionFactory::RegisterBlockAction(const std::string& name, BlockActionCreator creator)
{
    Registry()[ToLowerCopy(name)] = std::move(creator);
}

std::unique_ptr<TestAction> ActionFactory::CreateAction(const std::vector<std::string>& parts, const std::string& whole_line, int line_number)
{
    EnsureBuiltinsRegistered();

    if (!parts.empty()) {
        if (const auto iterator = Registry().find(ToLowerCopy(parts.front())); iterator != Registry().end()) {
            auto action = iterator->second(line_number, whole_line);
            if (!action) {
                throw TestException("Cannot create block action at line " + std::to_string(line_number));
            }
            action->Parse(parts);
            return action;
        }
    }

    return std::make_unique<TestAction>(line_number, whole_line);
}

} // namespace automationtest::testbooklib::action
