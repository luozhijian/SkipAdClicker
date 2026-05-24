#ifndef AUTOMATIOTEST_APP_SERVICES_APPLICATIONSERVICE_HPP
#define AUTOMATIOTEST_APP_SERVICES_APPLICATIONSERVICE_HPP

#include "ApplicationServiceBase.hpp"

#include "../../Utilities/Interface/IInteractiveAction.hpp"
#include "../../Utilities/Status/LoadFunctions.hpp"

#include <QProcess>

namespace automationtest::app::services {

class ApplicationService : public ApplicationServiceBase, public utilities::interface::IInteractiveAction {
public:
    static ApplicationService& Instance();

    static utilities::Bitmap OpenApplication(
        const std::string& open_new,
        double min_height_percent_of_screen,
        double seconds,
        const std::string& application,
        const std::string& parameters = {});

    static void RegisterBindings(utilities::status::LoadFunctions& load_functions);

    [[nodiscard]] utilities::Bitmap TakeScreenShot() override;
    [[nodiscard]] utilities::Bitmap TakeScreenShotAndPush() override;
    [[nodiscard]] utilities::Bitmap SimulateMouseClick(utilities::Point point, int milliseconds_to_wait) override;
    [[nodiscard]] utilities::Bitmap SimulateMouseRightClick(utilities::Point point, int milliseconds_to_wait) override;
    [[nodiscard]] utilities::Bitmap SimulateMouseClickSendInput(utilities::Point point, int milliseconds_to_wait) override;
    [[nodiscard]] utilities::Bitmap SimulateInputLetter(utilities::Point point, const std::string& letters, int milliseconds_to_wait) override;
    [[nodiscard]] utilities::Bitmap SimulateMouseClickSendInput2(utilities::Point point, int milliseconds_to_wait) override;
    [[nodiscard]] utilities::Bitmap SimulateMouseHoverOnePoint(utilities::Point point, int milliseconds_to_wait) override;
    [[nodiscard]] utilities::Bitmap SimulateInputKeyboardOnly(const std::string& letters, int milliseconds_to_wait) override;
    [[nodiscard]] utilities::Bitmap SimulateInputLetterUsingSendInput(const std::string& letters, int milliseconds_to_wait) override;
    [[nodiscard]] utilities::Bitmap MouseClickSendInputInTwoParts(
        utilities::Point point,
        const std::string& letters,
        const std::string& letters_using_send_input,
        int milliseconds_to_wait) override;

protected:
    ApplicationService() = default;

private:
    static utilities::Bitmap GrabPrimaryScreen();
    static void Wait(int milliseconds);
};

} // namespace automationtest::app::services

#endif // AUTOMATIOTEST_APP_SERVICES_APPLICATIONSERVICE_HPP
