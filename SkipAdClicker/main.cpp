#include "MainWindow.hpp"
#include "StartUp.hpp"
#include "SharedMemorySemaphore.hpp"
#include "../Utilities/OSRelated/SystemInfo.hpp"
#include "../Utilities/Logger.hpp"
#include <QApplication>
#include <QByteArray>
#include <QGuiApplication>
#include <QMessageBox>
#include <iostream>

namespace {

#if defined(__linux__)
void PrepareLinuxProcessEnvironment()
{
    qunsetenv("SESSION_MANAGER");

    const auto gtk_modules = qgetenv("GTK_MODULES");
    if (gtk_modules.isEmpty()) {
        return;
    }

    QByteArray filtered_modules;
    for (const auto& module : gtk_modules.split(':')) {
        const auto trimmed_module = module.trimmed();
        if (trimmed_module.isEmpty()
            || trimmed_module == "canberra-gtk-module"
            || trimmed_module == "canberra-gtk3-module") {
            continue;
        }
        if (!filtered_modules.isEmpty()) {
            filtered_modules.append(':');
        }
        filtered_modules.append(trimmed_module);
    }

    if (filtered_modules.isEmpty()) {
        qunsetenv("GTK_MODULES");
    } else {
        qputenv("GTK_MODULES", filtered_modules);
    }
}

#else
void PrepareLinuxProcessEnvironment()
{
}
#endif

void PrepareQtRenderingEnvironment()
{
#if defined(__linux__)
    if (qEnvironmentVariableIsEmpty("QT_XCB_NO_MITSHM")) {
        qputenv("QT_XCB_NO_MITSHM", "1");
    }
    if (qEnvironmentVariableIsEmpty("QT_WIDGETS_RHI")) {
        qputenv("QT_WIDGETS_RHI", "0");
    }
#endif

    QApplication::setAttribute(Qt::AA_ForceRasterWidgets, true);
    QApplication::setAttribute(Qt::AA_UseSoftwareOpenGL, true);
}

#if defined(__linux__)
bool IsWaylandSession()
{
    const auto session_type = qEnvironmentVariable("XDG_SESSION_TYPE").trimmed();
    if (session_type.compare("wayland", Qt::CaseInsensitive) == 0) {
        return true;
    }

    return QGuiApplication::platformName().contains("wayland", Qt::CaseInsensitive);
}

void WarnIfWaylandSession()
{
    if (!IsWaylandSession()) {
        return;
    }

    QMessageBox::warning(
        nullptr,
        "SkipAdClicker",
        "SkipAdClicker is running in a Wayland session.\n\n"
        "Screenshots and mouse automation may not work reliably in Wayland mode.\n\n"
        "For best results on Ubuntu, sign out, click the gear icon on the login screen, choose \"Ubuntu on Xorg\" (GNOME/Xorg), and sign in again.");
}
#else
void WarnIfWaylandSession()
{
}
#endif

} // namespace

int main(int argc, char* argv[])
{
    PrepareQtRenderingEnvironment();
    PrepareLinuxProcessEnvironment();

    automationtest::app::g_SharedMemorySemaphore =
        new automationtest::app::SharedMemorySemaphore(SystemInfo::GetCurrentProcessName());
    if (SystemInfo::IsAnotherProcessWithSameNameRunning() )
        automationtest::utilities::Logger::Debug( "Semaphore name: " + automationtest::app::g_SharedMemorySemaphore->Name() );
    if (SystemInfo::IsAnotherProcessWithSameNameRunning() )
    {
        automationtest::app::g_SharedMemorySemaphore->OpenForConsumer();
        automationtest::app::g_SharedMemorySemaphore->Trigger();
        automationtest::app::g_SharedMemorySemaphore->Close();
        delete automationtest::app::g_SharedMemorySemaphore;
        automationtest::app::g_SharedMemorySemaphore = nullptr;
        std::cout << "Another instance is already running, this instance will stop" << std::endl;
        return 0;
    }
    QApplication app(argc, argv);
    // QApplication::setApplicationName("SkipAdClicker");
    QApplication::setOrganizationName("SkipAdClicker");
    QApplication::setQuitOnLastWindowClosed(false);

    WarnIfWaylandSession();

    automationtest::app::StartUp::InitializeApplication();

    automationtest::app::MainWindow window;

    if (automationtest::app::StartUp::MinimizedWhenStarted()) {
        window.hide();
    } else {
        window.show();
    }
    window.ProcessCommandLine();

    int ret = app.exec();

    automationtest::app::ClearSharedMemorySemaphore();
    return ret;
}
