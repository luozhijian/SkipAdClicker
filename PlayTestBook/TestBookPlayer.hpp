#ifndef AUTOMATIOTEST_PLAYTESTBOOK_TESTBOOKPLAYER_HPP
#define AUTOMATIOTEST_PLAYTESTBOOK_TESTBOOKPLAYER_HPP

#include "../TestBookLib/FlowControls/DefFunction.hpp"
#include "../TestBookLib/TestBook.hpp"
#include "../Utilities/CommonTypes.hpp"
#include "../Utilities/Status/LoadFunctions.hpp"

#include <any>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace automationtest::playtestbook {

class TestBookPlayer {
public:
    explicit TestBookPlayer(std::shared_ptr<testbooklib::TestBook> test_book);

    void Assign();
    void Play(const std::function<bool()>& cancellation_requested = {});
    [[nodiscard]] const std::shared_ptr<testbooklib::TestBook>& Book() const noexcept;

    std::unordered_map<std::string, std::any, utilities::status::CaseInsensitiveHash, utilities::status::CaseInsensitiveEqual> book_controls {};
    std::unordered_map<std::string, std::any, utilities::status::CaseInsensitiveHash, utilities::status::CaseInsensitiveEqual> current_page_controls {};
    std::unordered_map<std::string, std::any, utilities::status::CaseInsensitiveHash, utilities::status::CaseInsensitiveEqual> current_action_controls {};
    std::vector<utilities::Bitmap> screenshots {};

private:
    using RegisteredMethod = utilities::status::LoadFunctions::RegisteredMethod;

    void CheckIsCancelled() const;
    void PlayOneAction(testbooklib::TestAction& action);
    void PlayOneActionWithoutBlock(testbooklib::TestAction& action);
    void ProcessSelfDefinedFunctions(testbooklib::TestAction& action, testbooklib::flowcontrols::DefFunction& self_defined_function);
    void GenerateParameterListForSelfDefinedFunction(
        testbooklib::TestAction& action,
        testbooklib::flowcontrols::DefFunction& self_defined_function,
        testbooklib::flowcontrols::DefFunction* previous_function);
    std::vector<std::any> GenerateParameterList(
        testbooklib::TestAction& action,
        const RegisteredMethod& method,
        testbooklib::flowcontrols::DefFunction* current_function);
    std::any GenerateParameter(
        const testbooklib::parameter::TestActionParameter& parameter,
        const utilities::status::LoadFunctions::RegisteredParameter& target_parameter,
        testbooklib::flowcontrols::DefFunction* current_function);
    void ProcessLetAction(testbooklib::TestAction& action, testbooklib::flowcontrols::DefFunction* current_function);

    std::shared_ptr<testbooklib::TestBook> test_book_ {};
    std::function<bool()> cancellation_requested_ {};
    std::vector<testbooklib::flowcontrols::DefFunction*> self_defined_functions_ {};
};

} // namespace automationtest::playtestbook

#endif // AUTOMATIOTEST_PLAYTESTBOOK_TESTBOOKPLAYER_HPP
