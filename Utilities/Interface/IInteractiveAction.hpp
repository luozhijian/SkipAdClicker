#ifndef AUTOMATIOTEST_INTERFACE_IINTERACTIVEACTION_HPP
#define AUTOMATIOTEST_INTERFACE_IINTERACTIVEACTION_HPP

#include "../CommonTypes.hpp"

#include <string>

namespace automationtest::utilities::interface {

class IInteractiveAction {
public:
    virtual ~IInteractiveAction() = default;

    [[nodiscard]] virtual automationtest::utilities::Bitmap TakeScreenShot() = 0;
    [[nodiscard]] virtual automationtest::utilities::Bitmap TakeScreenShotAndPush() = 0;
    [[nodiscard]] virtual automationtest::utilities::Bitmap SimulateMouseClick(automationtest::utilities::Point point, int milliseconds_to_wait) = 0;
    [[nodiscard]] virtual automationtest::utilities::Bitmap SimulateMouseRightClick(automationtest::utilities::Point point, int milliseconds_to_wait) = 0;
    [[nodiscard]] virtual automationtest::utilities::Bitmap SimulateMouseClickSendInput(automationtest::utilities::Point point, int milliseconds_to_wait) = 0;
    [[nodiscard]] virtual automationtest::utilities::Bitmap SimulateInputLetter(automationtest::utilities::Point point, const std::string& letters, int milliseconds_to_wait) = 0;
    [[nodiscard]] virtual automationtest::utilities::Bitmap SimulateMouseClickSendInput2(automationtest::utilities::Point point, int milliseconds_to_wait) = 0;
    [[nodiscard]] virtual automationtest::utilities::Bitmap SimulateMouseHoverOnePoint(automationtest::utilities::Point point, int milliseconds_to_wait) = 0;
    [[nodiscard]] virtual automationtest::utilities::Bitmap SimulateInputKeyboardOnly(const std::string& letters, int milliseconds_to_wait) = 0;
    [[nodiscard]] virtual automationtest::utilities::Bitmap SimulateInputLetterUsingSendInput(const std::string& letters, int milliseconds_to_wait) = 0;
    [[nodiscard]] virtual automationtest::utilities::Bitmap MouseClickSendInputInTwoParts(
        automationtest::utilities::Point point,
        const std::string& letters,
        const std::string& letters_using_send_input,
        int milliseconds_to_wait) = 0;
};

} // namespace automationtest::utilities::interface

#endif // AUTOMATIOTEST_INTERFACE_IINTERACTIVEACTION_HPP
