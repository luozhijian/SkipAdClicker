#include "MainWindow.hpp"
#include "StartUp.hpp"
#include "SharedMemorySemaphore.hpp"
#include "../Utilities/OSRelated/SystemInfo.hpp"
#include "../Utilities/Logger.hpp"
#include <QApplication>
#include <QByteArray>
#include <QGuiApplication>
#include <QIcon>
#include <QMessageBox>
#include <QTimer>
#include <QVersionNumber>
#include <QThread>
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

    QApplication app(argc, argv);
    QApplication::setApplicationName("SkipAdClicker");
    QApplication::setOrganizationName("SkipAdClicker");
    QApplication::setQuitOnLastWindowClosed(false);

    const auto current_version = automationtest::app::StartUp::VersionNumber();
    automationtest::app::g_SharedMemorySemaphore =
        new automationtest::app::SharedMemorySemaphore(SystemInfo::GetCurrentProcessName());
    const bool another_instance_running = automationtest::app::g_SharedMemorySemaphore->Exists();
    if (another_instance_running)
        automationtest::utilities::Logger::Debug( "Semaphore name: " + automationtest::app::g_SharedMemorySemaphore->Name() );
    if (another_instance_running)
    {
        automationtest::app::g_SharedMemorySemaphore->OpenForConsumer();
        const auto running_version = automationtest::app::g_SharedMemorySemaphore->RunningVersion();
        const bool current_is_newer =
            running_version.empty()
            || QVersionNumber::compare(
                QVersionNumber::fromString(QString::fromStdString(current_version)),
                QVersionNumber::fromString(QString::fromStdString(running_version))) > 0;

        if (current_is_newer)
        {
            automationtest::utilities::Logger::Debug(
                "Requesting version " + running_version + " to quit for upgrade to " + current_version);
            automationtest::app::g_SharedMemorySemaphore->RequestQuit();
        }
        else
        {
            automationtest::app::g_SharedMemorySemaphore->Trigger();
        }
        automationtest::app::g_SharedMemorySemaphore->Close();

        if (current_is_newer)
        {
            for (int attempt = 0;
                 attempt < 100 && automationtest::app::g_SharedMemorySemaphore->Exists();
                 ++attempt)
            {
                QThread::msleep(100);
            }

            if (automationtest::app::g_SharedMemorySemaphore->Exists())
            {
                automationtest::utilities::Logger::Debug(
                    "The previous instance did not quit in time; terminating it");
                if (SystemInfo::KillAnotherProcessWithSameName())
                {
                    for (int attempt = 0;
                         attempt < 20 && automationtest::app::g_SharedMemorySemaphore->Exists();
                         ++attempt)
                    {
                        QThread::msleep(100);
                    }

                    automationtest::app::g_SharedMemorySemaphore->Close();
#if defined(__linux__)
                    automationtest::app::g_SharedMemorySemaphore->Clear();
#endif
                }
                else
                {
                    automationtest::utilities::Logger::Error(
                        "Failed to terminate the previous instance");
                }
            }
        }

        if (automationtest::app::g_SharedMemorySemaphore->Exists())
        {
            delete automationtest::app::g_SharedMemorySemaphore;
            automationtest::app::g_SharedMemorySemaphore = nullptr;
            std::cout << "Another instance is already running, this instance will stop" << std::endl;
            return 0;
        }
    }

    if (!automationtest::app::g_SharedMemorySemaphore->CreateForCreator(current_version))
    {
        delete automationtest::app::g_SharedMemorySemaphore;
        automationtest::app::g_SharedMemorySemaphore = nullptr;
        std::cout << "Another instance started at the same time, this instance will stop" << std::endl;
        return 0;
    }

    auto application_icon = QIcon(QCoreApplication::applicationDirPath() + "/SkipAdClicker.ico");

    QApplication::setWindowIcon(application_icon);

    WarnIfWaylandSession();

    automationtest::app::StartUp::InitializeApplication();

    automationtest::app::MainWindow window;

    const bool start_minimized = automationtest::app::StartUp::MinimizedWhenStarted();
    if (!start_minimized) {
        window.show();
    }
    window.ProcessCommandLine();
    if (start_minimized) {
        QTimer::singleShot(0, &window, &automationtest::app::MainWindow::MinimizedApplication);
    }

    int ret = app.exec();

    automationtest::app::ClearSharedMemorySemaphore();
    return ret;
}
