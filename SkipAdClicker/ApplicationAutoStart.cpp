#include "ApplicationAutoStart.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QStandardPaths>
#include <QString>

#if defined(Q_OS_WIN)
#include <QSettings>
#endif

#if defined(Q_OS_MACOS)
#include <QXmlStreamWriter>
#endif

namespace automationtest::app {

namespace {

constexpr auto kApplicationName = "SkipAdClicker";

bool Fail(QString* error_message, const QString& message)
{
    if (error_message != nullptr) {
        *error_message = message;
    }
    return false;
}

bool EnsureParentDirectory(const QString& file_path, QString* error_message)
{
    QDir directory = QFileInfo(file_path).absoluteDir();
    if (directory.exists() || directory.mkpath(".")) {
        return true;
    }
    return Fail(error_message, QString("Could not create startup directory: %1").arg(directory.absolutePath()));
}

bool WriteFile(const QString& file_path, const QByteArray& contents, QString* error_message)
{
    if (!EnsureParentDirectory(file_path, error_message)) {
        return false;
    }

    QSaveFile file(file_path);
    file.setDirectWriteFallback(true);
    if (!file.open(QIODevice::WriteOnly)) {
        return Fail(error_message, QString("Could not open startup file %1: %2").arg(file_path, file.errorString()));
    }
    if (file.write(contents) != contents.size()) {
        return Fail(error_message, QString("Could not write startup file %1: %2").arg(file_path, file.errorString()));
    }
    if (!file.commit()) {
        return Fail(error_message, QString("Could not save startup file %1: %2").arg(file_path, file.errorString()));
    }
    return true;
}

bool RemoveFile(const QString& file_path, QString* error_message)
{
    if (!QFileInfo::exists(file_path) || QFile::remove(file_path)) {
        return true;
    }
    return Fail(error_message, QString("Could not remove startup file: %1").arg(file_path));
}

#if defined(Q_OS_WIN)
QSettings WindowsRunSettings()
{
    return QSettings(
        "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        QSettings::NativeFormat);
}

QString WindowsStartupCommand()
{
    const auto executable = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    if (executable.isEmpty()) {
        return {};
    }
    return QString("\"%1\"").arg(executable);
}

bool IsPlatformRegistrationEnabled()
{
    auto run_settings = WindowsRunSettings();
    if (!run_settings.contains(kApplicationName)) {
        return false;
    }

    const auto expected_command = WindowsStartupCommand();
    if (!expected_command.isEmpty()
        && run_settings.value(kApplicationName).toString() != expected_command) {
        run_settings.setValue(kApplicationName, expected_command);
        run_settings.sync();
    }
    return run_settings.status() == QSettings::NoError;
}

bool UpdatePlatformRegistration(bool enabled, QString* error_message)
{
    auto run_settings = WindowsRunSettings();

    if (enabled) {
        const auto command = WindowsStartupCommand();
        if (command.isEmpty()) {
            return Fail(error_message, "Could not determine the Windows startup command.");
        }
        run_settings.setValue(kApplicationName, command);
    } else {
        run_settings.remove(kApplicationName);
    }

    run_settings.sync();
    if (run_settings.status() != QSettings::NoError) {
        return Fail(error_message, "Could not update the Windows startup registry.");
    }
    return true;
}
#elif defined(Q_OS_MACOS)
QString LaunchAgentPath()
{
    return QDir::home().absoluteFilePath("Library/LaunchAgents/com.skipadclicker.SkipAdClicker.plist");
}

bool IsPlatformRegistrationEnabled()
{
    return QFileInfo::exists(LaunchAgentPath());
}

QByteArray LaunchAgentContents()
{
    QByteArray contents;
    QXmlStreamWriter xml(&contents);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" "
                 "\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">");
    xml.writeStartElement("plist");
    xml.writeAttribute("version", "1.0");
    xml.writeStartElement("dict");
    xml.writeTextElement("key", "Label");
    xml.writeTextElement("string", "com.skipadclicker.SkipAdClicker");
    xml.writeTextElement("key", "ProgramArguments");
    xml.writeStartElement("array");
    xml.writeTextElement("string", QCoreApplication::applicationFilePath());
    xml.writeEndElement();
    xml.writeTextElement("key", "RunAtLoad");
    xml.writeEmptyElement("true");
    xml.writeEndElement();
    xml.writeEndElement();
    xml.writeEndDocument();
    return contents;
}

bool UpdatePlatformRegistration(bool enabled, QString* error_message)
{
    const auto file_path = LaunchAgentPath();
    return enabled
        ? WriteFile(file_path, LaunchAgentContents(), error_message)
        : RemoveFile(file_path, error_message);
}
#elif defined(Q_OS_LINUX)
QString LinuxAutoStartPath()
{
    const auto snap_user_data = qEnvironmentVariable("SNAP_USER_DATA").trimmed();
    if (!snap_user_data.isEmpty()) {
        return QDir(snap_user_data).absoluteFilePath(".config/autostart/skipadclicker.desktop");
    }

    auto config_path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    if (config_path.isEmpty()) {
        config_path = QDir::home().absoluteFilePath(".config");
    }
    return QDir(config_path).absoluteFilePath("autostart/skipadclicker.desktop");
}

bool IsPlatformRegistrationEnabled()
{
    return QFileInfo::exists(LinuxAutoStartPath());
}

QString QuoteDesktopExecArgument(QString value)
{
    value.replace("\\", "\\\\");
    value.replace("\"", "\\\"");
    value.replace("`", "\\`");
    value.replace("$", "\\$");
    return QString("\"%1\"").arg(value);
}

QByteArray LinuxDesktopEntry()
{
    const bool is_snap = !qEnvironmentVariable("SNAP").trimmed().isEmpty();
    const auto command = is_snap
        ? QString("skipadclicker")
        : QuoteDesktopExecArgument(QCoreApplication::applicationFilePath());

    return QString(
               "[Desktop Entry]\n"
               "Type=Application\n"
               "Name=SkipAdClicker\n"
               "Exec=%1\n"
               "Terminal=false\n"
               "NoDisplay=true\n"
               "X-GNOME-Autostart-enabled=true\n")
        .arg(command)
        .toUtf8();
}

bool UpdatePlatformRegistration(bool enabled, QString* error_message)
{
    const auto file_path = LinuxAutoStartPath();
    return enabled
        ? WriteFile(file_path, LinuxDesktopEntry(), error_message)
        : RemoveFile(file_path, error_message);
}
#else
bool IsPlatformRegistrationEnabled()
{
    return false;
}

bool UpdatePlatformRegistration(bool, QString* error_message)
{
    return Fail(error_message, "Starting after restart is not supported on this platform.");
}
#endif

} // namespace

bool ApplicationAutoStart::IsEnabled()
{
    return IsPlatformRegistrationEnabled();
}

bool ApplicationAutoStart::SetEnabled(bool enabled, QString* error_message)
{
    return UpdatePlatformRegistration(enabled, error_message);
}

} // namespace automationtest::app
