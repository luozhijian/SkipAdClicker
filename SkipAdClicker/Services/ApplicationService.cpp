#include "ApplicationService.hpp"

#include "../LogView.hpp"
#include "../StartUp.hpp"
#include "../../PlayTestBook/Services/RecentActivityService.hpp"
#include "../../Utilities/DependencyInjection/DependencyStore.hpp"
#include "../../Utilities/Exceptions/TestException.hpp"

#include <QApplication>
#include <QCursor>
#include <QElapsedTimer>
#include <QGuiApplication>
#include <QImage>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPixmap>
#include <QScreen>
#include <QThread>

#include <cstring>

namespace automationtest::app::services {

namespace {

std::string AnyToString(const std::any& value)
{
    if (!value.has_value()) {
        return {};
    }
    if (value.type() == typeid(std::string)) {
        return std::any_cast<std::string>(value);
    }
    if (value.type() == typeid(const char*)) {
        return std::any_cast<const char*>(value);
    }
    return {};
}

double AnyToDouble(const std::any& value)
{
    if (value.type() == typeid(double)) {
        return std::any_cast<double>(value);
    }
    if (value.type() == typeid(int)) {
        return static_cast<double>(std::any_cast<int>(value));
    }
    if (value.type() == typeid(std::string)) {
        return std::stod(std::any_cast<std::string>(value));
    }
    return 0.0;
}

int AnyToInt(const std::any& value)
{
    if (value.type() == typeid(int)) {
        return std::any_cast<int>(value);
    }
    if (value.type() == typeid(double)) {
        return static_cast<int>(std::any_cast<double>(value));
    }
    if (value.type() == typeid(std::string)) {
        return std::stoi(std::any_cast<std::string>(value));
    }
    return 0;
}

utilities::Point AnyToPoint(const std::any& value)
{
    if (value.type() == typeid(utilities::Point)) {
        return std::any_cast<utilities::Point>(value);
    }
    return {};
}

} // namespace

ApplicationService& ApplicationService::Instance()
{
    static ApplicationService instance;
    return instance;
}

utilities::Bitmap ApplicationService::OpenApplication(
    const std::string&,
    double,
    double seconds,
    const std::string& application,
    const std::string& parameters)
{
    StartUp::RegisterInteractiveAction(&Instance());

    QStringList arguments;
    if (!parameters.empty()) {
        arguments = QString::fromStdString(parameters).split(' ', Qt::SkipEmptyParts);
    }
    QProcess::startDetached(QString::fromStdString(application), arguments);
    Wait(static_cast<int>(seconds * 1000.0));
    return Instance().TakeScreenShot();
}

void ApplicationService::RegisterBindings(utilities::status::LoadFunctions& load_functions)
{
    using Method = utilities::status::LoadFunctions::RegisteredMethod;
    using Parameter = utilities::status::LoadFunctions::RegisteredParameter;

    auto string_parameter = [](const std::string& name, bool has_default = false, std::string default_value = {}) {
        return Parameter {
            .name = name,
            .converter = [](const std::string& value) -> std::any {
                return value;
            },
            .default_value = std::move(default_value),
            .has_default_value = has_default,
        };
    };
    auto int_parameter = [](const std::string& name, int default_value = 0) {
        return Parameter {
            .name = name,
            .converter = [](const std::string& value) -> std::any {
                return AnyToInt(std::any(value));
            },
            .default_value = default_value,
            .has_default_value = true,
        };
    };
    auto point_parameter = [](const std::string& name) {
        return Parameter {
            .name = name,
            .converter = [](const std::string& value) -> std::any {
                return AnyToPoint(std::any(value));
            },
        };
    };

    load_functions.RegisterMethod("OpenApplication", Method {
        .declaring_type = "ApplicationService",
        .invoke = [](const std::vector<std::any>& args) -> std::any {
            const std::string parameters =
                (args.size() > 4 ? AnyToString(args[4]) : "") + " " +
                (args.size() > 5 ? AnyToString(args[5]) : "") + " " +
                (args.size() > 6 ? AnyToString(args[6]) : "") + " " +
                (args.size() > 7 ? AnyToString(args[7]) : "") + " " +
                (args.size() > 8 ? AnyToString(args[8]) : "");
            return ApplicationService::OpenApplication(
                AnyToString(args[0]),
                AnyToDouble(args[1]),
                AnyToDouble(args[2]),
                AnyToString(args[3]),
                parameters);
        },
        .parameters = {
            string_parameter("openNew"),
            Parameter {.name = "minHeightOfPercentOfScreen", .converter = [](const std::string& value) -> std::any { return AnyToDouble(std::any(value)); }},
            Parameter {.name = "seconds", .converter = [](const std::string& value) -> std::any { return AnyToDouble(std::any(value)); }},
            string_parameter("application"),
            string_parameter("parameter1", true),
            string_parameter("parameter2", true),
            string_parameter("parameter3", true),
            string_parameter("parameter4", true),
            string_parameter("parameter5", true),
        },
    });

    load_functions.RegisterMethod("SendKeyPageDown", Method {
        .declaring_type = "ApplicationService",
        .invoke = [](const std::vector<std::any>& args) -> std::any {
            return ApplicationService::Instance().SimulateInputKeyboardOnly("\\PgDn", args.size() > 1 ? AnyToInt(args[1]) : 500);
        },
        .parameters = {point_parameter("p"), int_parameter("timeoutInMs", 500)},
    });

    load_functions.RegisterMethod("Simulate_InputKeyBoardOnly", Method {
        .declaring_type = "ApplicationService",
        .invoke = [](const std::vector<std::any>& args) -> std::any {
            return ApplicationService::Instance().SimulateInputKeyboardOnly(AnyToString(args[0]), args.size() > 1 ? AnyToInt(args[1]) : 100);
        },
        .parameters = {string_parameter("letters"), int_parameter("milliSecondsToWait", 100)},
    });

    load_functions.RegisterMethod("Simulate_InputLetter", Method {
        .declaring_type = "ApplicationService",
        .invoke = [](const std::vector<std::any>& args) -> std::any {
            return ApplicationService::Instance().SimulateInputLetter(AnyToPoint(args[0]), AnyToString(args[1]), args.size() > 2 ? AnyToInt(args[2]) : 500);
        },
        .parameters = {point_parameter("p"), string_parameter("letters"), int_parameter("timeoutInMs", 500)},
    });
}

utilities::Bitmap ApplicationService::TakeScreenShot()
{
    return GrabPrimaryScreen();
}

utilities::Bitmap ApplicationService::TakeScreenShotAndPush()
{
    auto bitmap = TakeScreenShot();
    playtestbook::services::RecentActivityService::Instance().PushBitmap(bitmap);
    return bitmap;
}

utilities::Bitmap ApplicationService::SimulateMouseClick(utilities::Point point, int milliseconds_to_wait)
{
    playtestbook::services::RecentActivityService::Instance().PushLocation(point);
    if (auto* target = qApp->activeWindow()) {
        const QPoint local(point.x, point.y);
        QCursor::setPos(target->mapToGlobal(local));
        QMouseEvent press(QEvent::MouseButtonPress, local, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QMouseEvent release(QEvent::MouseButtonRelease, local, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
        QApplication::sendEvent(target, &press);
        QApplication::sendEvent(target, &release);
    }
    Wait(milliseconds_to_wait);
    return TakeScreenShot();
}

utilities::Bitmap ApplicationService::SimulateMouseRightClick(utilities::Point point, int milliseconds_to_wait)
{
    playtestbook::services::RecentActivityService::Instance().PushLocation(point);
    if (auto* target = qApp->activeWindow()) {
        const QPoint local(point.x, point.y);
        QCursor::setPos(target->mapToGlobal(local));
        QMouseEvent press(QEvent::MouseButtonPress, local, Qt::RightButton, Qt::RightButton, Qt::NoModifier);
        QMouseEvent release(QEvent::MouseButtonRelease, local, Qt::RightButton, Qt::NoButton, Qt::NoModifier);
        QApplication::sendEvent(target, &press);
        QApplication::sendEvent(target, &release);
    }
    Wait(milliseconds_to_wait);
    return TakeScreenShot();
}

utilities::Bitmap ApplicationService::SimulateMouseClickSendInput(utilities::Point point, int milliseconds_to_wait)
{
    return SimulateMouseClick(point, milliseconds_to_wait);
}

utilities::Bitmap ApplicationService::SimulateInputLetter(utilities::Point point, const std::string& letters, int milliseconds_to_wait)
{
    SimulateMouseClick(point, 50);
    return SimulateInputKeyboardOnly(letters, milliseconds_to_wait);
}

utilities::Bitmap ApplicationService::SimulateMouseClickSendInput2(utilities::Point point, int milliseconds_to_wait)
{
    return SimulateMouseClick(point, milliseconds_to_wait);
}

utilities::Bitmap ApplicationService::SimulateMouseHoverOnePoint(utilities::Point point, int milliseconds_to_wait)
{
    if (auto* target = qApp->activeWindow()) {
        const QPoint local(point.x, point.y);
        QCursor::setPos(target->mapToGlobal(local));
        QMouseEvent move(QEvent::MouseMove, local, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
        QApplication::sendEvent(target, &move);
    }
    Wait(milliseconds_to_wait);
    return TakeScreenShot();
}

utilities::Bitmap ApplicationService::SimulateInputKeyboardOnly(const std::string& letters, int milliseconds_to_wait)
{
    if (auto* target = qApp->activeWindow()) {
        const auto text = QString::fromStdString(letters);
        for (const auto character : text) {
            QKeyEvent press(QEvent::KeyPress, 0, Qt::NoModifier, QString(character));
            QKeyEvent release(QEvent::KeyRelease, 0, Qt::NoModifier, QString(character));
            QApplication::sendEvent(target, &press);
            QApplication::sendEvent(target, &release);
        }
    }
    Wait(milliseconds_to_wait);
    return TakeScreenShot();
}

utilities::Bitmap ApplicationService::SimulateInputLetterUsingSendInput(const std::string& letters, int milliseconds_to_wait)
{
    return SimulateInputKeyboardOnly(letters, milliseconds_to_wait);
}

utilities::Bitmap ApplicationService::MouseClickSendInputInTwoParts(
    utilities::Point point,
    const std::string& letters,
    const std::string& letters_using_send_input,
    int milliseconds_to_wait)
{
    SimulateInputLetter(point, letters, 50);
    return SimulateInputKeyboardOnly(letters_using_send_input, milliseconds_to_wait);
}

utilities::Bitmap ApplicationService::GrabPrimaryScreen()
{
    auto* screen = QGuiApplication::primaryScreen();
    if (screen == nullptr) {
        return {};
    }

    const auto image = screen->grabWindow(0).toImage().convertToFormat(QImage::Format_RGBA8888);
    utilities::Bitmap result;
    result.width = image.width();
    result.height = image.height();
    result.channels = 4;
    result.stride = image.bytesPerLine();
    result.pixels.resize(static_cast<std::size_t>(image.sizeInBytes()));
    std::memcpy(result.pixels.data(), image.constBits(), static_cast<std::size_t>(image.sizeInBytes()));
    return result;
}

void ApplicationService::Wait(int milliseconds)
{
    if (milliseconds <= 0) {
        return;
    }
    QThread::msleep(static_cast<unsigned long>(milliseconds));
}

} // namespace automationtest::app::services
