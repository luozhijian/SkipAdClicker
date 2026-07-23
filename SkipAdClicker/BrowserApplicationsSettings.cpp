#include "BrowserApplicationsSettings.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QMap>
#include <QSettings>
#include <QStandardPaths>
#include <QVariant>

#include <algorithm>
#include <vector>

#include "../Utilities/StringLib.hpp"

using automationtest::utilities::StringLib;

namespace automationtest::app {

namespace {

constexpr auto kSettingsFileName = "BrowserApplications.ini";
std::unordered_set<std::wstring> browser_applications {};

QString InstalledFilePath()
{
    return QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(kSettingsFileName);
}

void SetError(QString* error_message, const QString& message)
{
    if (error_message != nullptr) {
        *error_message = message;
    }
}

QString LegacyCurrentPlatformPrefix()
{
#if defined(Q_OS_WIN)
    return QStringLiteral("Windows/");
#elif defined(Q_OS_LINUX)
    return QStringLiteral("Linux/");
#elif defined(Q_OS_MACOS)
    return QStringLiteral("macOS/");
#else
    return {};
#endif
}

bool IsLegacyPlatformKey(const QString& key)
{
    return key.startsWith(QStringLiteral("Windows/"))
        || key.startsWith(QStringLiteral("Linux/"))
        || key.startsWith(QStringLiteral("macOS/"));
}

void UpdateBrowserApplications(const QMap<QString, QVariant>& settings)
{
    std::unordered_set<std::wstring> enabled_applications;
    const auto applications_prefix = QStringLiteral("Applications/");

    for (auto iterator = settings.cbegin(); iterator != settings.cend(); ++iterator) {
        if (!iterator.key().startsWith(applications_prefix) || !iterator.value().toBool()) {
            continue;
        }
        enabled_applications.insert(StringLib::ToLower(iterator.key().mid(applications_prefix.size()).toStdWString()));
    }

    browser_applications = std::move(enabled_applications);
}

} // namespace

QString BrowserApplicationsSettings::UserFilePath()
{
    auto directory_path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (directory_path.isEmpty()) {
        directory_path = QDir::home().absoluteFilePath(".skipadclicker");
    }
    return QDir(directory_path).absoluteFilePath(kSettingsFileName);
}

bool BrowserApplicationsSettings::MergeWithInstalledDefaults(QString* error_message)
{
    const auto installed_file_path = InstalledFilePath();
    if (!QFileInfo::exists(installed_file_path)) {
        SetError(error_message, QString("The installed browser settings file does not exist:\n%1").arg(installed_file_path));
        return false;
    }

    const auto user_file_path = UserFilePath();
    const QFileInfo user_file_info(user_file_path);
    if (!QDir().mkpath(user_file_info.absolutePath())) {
        SetError(error_message, QString("Could not create the browser settings directory:\n%1").arg(user_file_info.absolutePath()));
        return false;
    }

    // QMap gives QSettings a deterministic, alphabetically sorted key order.
    // Installed defaults are loaded first; existing user values override them.
    QMap<QString, QVariant> merged_settings;
    {
        QSettings installed_settings(installed_file_path, QSettings::IniFormat);
        for (const auto& key : installed_settings.allKeys()) {
            merged_settings.insert(key, installed_settings.value(key));
        }
        if (installed_settings.status() != QSettings::NoError) {
            SetError(error_message, QString("Could not read the installed browser settings:\n%1").arg(installed_file_path));
            return false;
        }
    }

    if (user_file_info.exists()) {
        QSettings user_settings(user_file_path, QSettings::IniFormat);
        const auto legacy_platform_prefix = LegacyCurrentPlatformPrefix();

        // Migrate the current platform from the former three-section format.
        // Entries already stored under Applications take precedence below.
        for (const auto& key : user_settings.allKeys()) {
            if (!legacy_platform_prefix.isEmpty() && key.startsWith(legacy_platform_prefix)) {
                const auto identifier = key.mid(legacy_platform_prefix.size());
                merged_settings.insert(QStringLiteral("Applications/") + identifier, user_settings.value(key));
            }
        }
        for (const auto& key : user_settings.allKeys()) {
            if (!IsLegacyPlatformKey(key)) {
                merged_settings.insert(key, user_settings.value(key));
            }
        }
        if (user_settings.status() != QSettings::NoError) {
            SetError(error_message, QString("Could not read the user browser settings:\n%1").arg(user_file_path));
            return false;
        }
    }

    QSettings output_settings(user_file_path, QSettings::IniFormat);
    output_settings.clear();
    for (auto iterator = merged_settings.cbegin(); iterator != merged_settings.cend(); ++iterator) {
        output_settings.setValue(iterator.key(), iterator.value());
    }
    output_settings.sync();
    if (output_settings.status() != QSettings::NoError) {
        SetError(error_message, QString("Could not write the user browser settings:\n%1").arg(user_file_path));
        return false;
    }

    UpdateBrowserApplications(merged_settings);
    return true;
}

bool BrowserApplicationsSettings::Load(QList<BrowserApplicationEntry>& entries, QString* error_message)
{
    entries.clear();
    QSettings settings(UserFilePath(), QSettings::IniFormat);
    settings.beginGroup(QStringLiteral("Applications"));
    const auto identifiers = settings.childKeys();
    for (const auto& identifier : identifiers) {
        entries.append(BrowserApplicationEntry {
            .identifier = identifier,
            .enabled = settings.value(identifier, true).toBool(),
        });
    }
    settings.endGroup();

    if (settings.status() != QSettings::NoError) {
        SetError(error_message, QString("Could not read the user browser settings:\n%1").arg(UserFilePath()));
        return false;
    }

    std::sort(entries.begin(), entries.end(), [](const auto& left, const auto& right) {
        return QString::compare(left.identifier, right.identifier, Qt::CaseInsensitive) < 0;
    });
    return true;
}

bool BrowserApplicationsSettings::LoadInstalledIdentifiers(QStringList& identifiers, QString* error_message)
{
    identifiers.clear();
    QSettings settings(InstalledFilePath(), QSettings::IniFormat);
    settings.beginGroup(QStringLiteral("Applications"));
    identifiers = settings.childKeys();
    settings.endGroup();

    if (settings.status() != QSettings::NoError) {
        SetError(error_message, QString("Could not read the installed browser settings:\n%1").arg(InstalledFilePath()));
        return false;
    }
    return true;
}

bool BrowserApplicationsSettings::Save(const QList<BrowserApplicationEntry>& entries, QString* error_message)
{
    auto sorted_entries = entries;
    std::sort(sorted_entries.begin(), sorted_entries.end(), [](const auto& left, const auto& right) {
        return QString::compare(left.identifier, right.identifier, Qt::CaseInsensitive) < 0;
    });

    QSettings settings(UserFilePath(), QSettings::IniFormat);
    settings.beginGroup(QStringLiteral("Applications"));
    settings.remove(QString());
    for (const auto& entry : sorted_entries) {
        settings.setValue(entry.identifier, entry.enabled);
    }
    settings.endGroup();
    settings.sync();

    if (settings.status() != QSettings::NoError) {
        SetError(error_message, QString("Could not write the user browser settings:\n%1").arg(UserFilePath()));
        return false;
    }

    browser_applications.clear();
    browser_applications.reserve(static_cast<std::size_t>(sorted_entries.size()));
    for (const auto& entry : sorted_entries) {
        if (entry.enabled) {
            browser_applications.insert(StringLib::ToLower(entry.identifier.toStdWString()));
        }
    }
    return true;
}

std::any BrowserApplicationsSettings::GetBrowserApplications()
{
    return std::any {&browser_applications};
}

void BrowserApplicationsSettings::RegisterBindings(utilities::status::LoadFunctions& load_functions)
{
    load_functions.RegisterMethod("GetBrowserApplications", utilities::status::LoadFunctions::RegisteredMethod{
        .declaring_type = "GetBrowserApplications",
        .invoke = [](const std::vector<std::any>&) -> std::any {
            return GetBrowserApplications();
        },
        });
}



} // namespace automationtest::app
