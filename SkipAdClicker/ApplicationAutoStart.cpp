#include "ApplicationAutoStart.hpp"
#include "LogView.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QStandardPaths>
#include <QString>

#include <vector>

#if defined(Q_OS_WIN)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
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
constexpr auto kWindowsRunKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
constexpr auto kWindowsRunValue = L"SkipAdClicker";

QString WindowsErrorMessage(LSTATUS status)
{
    wchar_t* message = nullptr;
    const auto length = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER
            | FORMAT_MESSAGE_FROM_SYSTEM
            | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        static_cast<DWORD>(status),
        0,
        reinterpret_cast<wchar_t*>(&message),
        0,
        nullptr);
    const auto result = length != 0 && message != nullptr
        ? QString::fromWCharArray(message, static_cast<qsizetype>(length)).trimmed()
        : QString("Windows error %1").arg(status);
    if (message != nullptr) {
        LocalFree(message);
    }
    return result;
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
    HKEY run_key = nullptr;
    const auto open_status = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        kWindowsRunKey,
        0,
        KEY_QUERY_VALUE,
        &run_key);
    if (open_status != ERROR_SUCCESS) {
        return false;
    }

    const auto query_status = RegQueryValueExW(
        run_key,
        kWindowsRunValue,
        nullptr,
        nullptr,
        nullptr,
        nullptr);
    RegCloseKey(run_key);
    return query_status == ERROR_SUCCESS;
}

bool UpdatePlatformRegistration(bool enabled, QString* error_message)
{
    HKEY run_key = nullptr;
    const auto open_status = RegCreateKeyExW(
        HKEY_CURRENT_USER,
        kWindowsRunKey,
        0,
        nullptr,
        REG_OPTION_NON_VOLATILE,
        KEY_SET_VALUE | KEY_QUERY_VALUE,
        nullptr,
        &run_key,
        nullptr);
    if (open_status != ERROR_SUCCESS) {
        return Fail(
            error_message,
            QString("Could not open the Windows startup registry key: %1")
                .arg(WindowsErrorMessage(open_status)));
    }

    LSTATUS update_status = ERROR_SUCCESS;
    if (enabled) {
        const auto command = WindowsStartupCommand();
        if (command.isEmpty()) {
            RegCloseKey(run_key);
            return Fail(error_message, "Could not determine the Windows startup command.");
        }

        const auto command_utf16 = command.toStdWString();
        update_status = RegSetValueExW(
            run_key,
            kWindowsRunValue,
            0,
            REG_SZ,
            reinterpret_cast<const BYTE*>(command_utf16.c_str()),
            static_cast<DWORD>((command_utf16.size() + 1) * sizeof(wchar_t)));
        if (update_status != ERROR_SUCCESS) {
            RegCloseKey(run_key);
            return Fail(
                error_message,
                QString("RegSetValueExW failed: %1")
                    .arg(WindowsErrorMessage(update_status)));
        }

        DWORD value_type = 0;
        DWORD value_size = 0;
        auto verify_status = RegQueryValueExW(
            run_key,
            kWindowsRunValue,
            nullptr,
            &value_type,
            nullptr,
            &value_size);
        std::vector<wchar_t> stored_value;
        if (verify_status == ERROR_SUCCESS && value_type == REG_SZ && value_size > 0) {
            stored_value.resize((value_size / sizeof(wchar_t)) + 1);
            verify_status = RegQueryValueExW(
                run_key,
                kWindowsRunValue,
                nullptr,
                &value_type,
                reinterpret_cast<BYTE*>(stored_value.data()),
                &value_size);
        }
        if (verify_status != ERROR_SUCCESS
            || value_type != REG_SZ
            || QString::fromWCharArray(stored_value.data()) != command) {
            RegCloseKey(run_key);
            return Fail(
                error_message,
                verify_status == ERROR_SUCCESS
                    ? "The Windows startup registry value did not match after it was written."
                    : QString("Could not verify the Windows startup registry value: %1")
                          .arg(WindowsErrorMessage(verify_status)));
        }

        LogView::AddLog(QString("Run at Startup: ") + kApplicationName + QString(" ") + command);
    } else {
        update_status = RegDeleteValueW(run_key, kWindowsRunValue);
        if (update_status == ERROR_FILE_NOT_FOUND) {
            update_status = ERROR_SUCCESS;
        }
    }
    RegCloseKey(run_key);

    if (update_status != ERROR_SUCCESS) {
        return Fail(
            error_message,
            QString("Could not update the Windows startup registry: %1")
                .arg(WindowsErrorMessage(update_status)));
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
