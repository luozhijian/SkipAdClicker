#include "MainWindow.hpp"
#include "StartUp.hpp"

#include <QApplication>
#include <QLockFile>
#include <QDir>
#include <QSettings>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("SkipAdClicker");
    QApplication::setOrganizationName("SkipAdClicker");
    QApplication::setQuitOnLastWindowClosed(false);

    const bool allow_only_one_instance = automationtest::app::StartUp::AppSettings().value("AllowOnlyOneInstance", true).toBool();
    std::unique_ptr<QLockFile> lock_file;
    if (allow_only_one_instance) {
        lock_file = std::make_unique<QLockFile>(QDir::temp().filePath("SkipAdClicker.MainWindow.SingleInstance.lock"));
        lock_file->setStaleLockTime(0);
        if (!lock_file->tryLock(100)) {
            return 0;
        }
    }

    automationtest::app::StartUp::InitializeApplication();

    automationtest::app::MainWindow window;
    window.show();
    window.ProcessCommandLine();

    return app.exec();
}
