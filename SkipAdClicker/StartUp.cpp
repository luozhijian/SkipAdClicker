#include "StartUp.hpp"

#include "FullScreenControls.hpp"
#include "BrowserApplicationsSettings.hpp"
#include "LogView.hpp"
#include "RecentFiles.hpp"
#include "Services/ApplicationService.hpp"
#include "Services/ChromeService.hpp"

#include "../OcrLib/OcrProcesser.hpp"
#include "../PlayTestBook/Services/RecentActivityService.hpp"
#include "../PlayTestBook/Services/StaticFunctionService.hpp"
#include "../PlayTestBook/Services/StaticResourceService.hpp"
#include "../PlayTestBook/TestBookPlayer.hpp"
#include "../TestBookLib/Parser/TestBookParser.hpp"
#include "../Utilities/DependencyInjection/DependencyStore.hpp"
#include "../Utilities/FilePathLib.hpp"
#include "../Utilities/GlobalSetting.hpp"
#include "../Utilities/GlobalVariables.hpp"
#include "../Utilities/Logger.hpp"
#include "../Utilities/Services/VariableService.hpp"
#include "../Utilities/Status/LoadFunctions.hpp"
#include "../UserInterfaceLib/SkipAdDetector.hpp"
#include "../Utilities/OSRelated/OSRelatedFunctions.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QString>

namespace automationtest::app {

namespace {

constexpr auto kMinimizedWhenStartedSetting = "MinimizedWhenStarted";
constexpr auto kAutoStartPromptShownSetting = "AutoStartPromptShown";

std::string ResolvePath(const QString& value)
{
    if (value.isEmpty()) {
        return {};
    }
    QFileInfo info(value);
    if (info.isAbsolute()) {
        return info.absoluteFilePath().toStdString();
    }
    return QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(value).toStdString();
}

bool IsSnapEnvironment()
{
    return !qEnvironmentVariable("SNAP").trimmed().isEmpty();
}

QString SnapWritableDirectory()
{
    auto path = qEnvironmentVariable("SNAP_USER_COMMON").trimmed();
    if (path.isEmpty()) {
        path = qEnvironmentVariable("SNAP_USER_DATA").trimmed();
    }
    if (path.isEmpty()) {
        path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    }
    if (path.isEmpty()) {
        path = QDir::home().absoluteFilePath(".skipadclicker");
    }

    QDir directory(path);
    directory.mkpath(".");
    return directory.absolutePath();
}

std::string ResolveWritablePath(const QString& value)
{
    if (value.isEmpty()) {
        return {};
    }

    QFileInfo info(value);
    if (info.isAbsolute()) {
        return info.absoluteFilePath().toStdString();
    }

    const auto base_path = IsSnapEnvironment() ? SnapWritableDirectory() : QCoreApplication::applicationDirPath();
    return QDir(base_path).absoluteFilePath(value).toStdString();
}

QVariant ReadSetting(const QString& key, const QVariant& default_value = {})
{
    auto settings = StartUp::AppSettings();
    if (settings.contains(key)) {
        return settings.value(key, default_value);
    }
    return QSettings().value(key, default_value);
}
void ConfigureLogging()
{
    auto settings = StartUp::AppSettings();
    settings.beginGroup("Logging");

    utilities::LogSettings log_settings {};
    log_settings.enabled = settings.value("Enabled", true).toBool();
    log_settings.minimum_level = utilities::Logger::ParseLevel(settings.value("Level", "Info").toString().toStdString());
    log_settings.file_path = ResolveWritablePath(settings.value("File", "./Logs/SkipAdClicker.log").toString());
    log_settings.write_to_console = settings.value("Console", false).toBool();

    settings.endGroup();
    utilities::Logger::Configure(log_settings);
    utilities::Logger::SetLogToView([](std::string message) {
        LogView::AddLog(QString::fromStdString(message));
    });
}

void ConfigureDebugSettings()
{
    auto settings = StartUp::AppSettings();
    settings.beginGroup("Debug");

    auto debug_view_images_location = settings.value("DebugViewImagesLocation").toString();
    const auto tools_to_view_bitmap = settings.value("ToolsToViewBitmap").toString().trimmed().toStdString();

    settings.endGroup();

    if (debug_view_images_location.trimmed().isEmpty()) {
        debug_view_images_location = ReadSetting("DebugImagesLocation").toString();
    }

    utilities::GlobalSetting::SetDebugViewImageFileFolder(ResolveWritablePath(debug_view_images_location));
    utilities::GlobalSetting::SetToolsToViewBitmap(tools_to_view_bitmap);
}

} // namespace

void StartUp::InitializeApplication()
{
    auto& variable_service = utilities::services::VariableService::Instance();
    testbooklib::SetVariableService(&variable_service);
    utilities::GlobalSetting::current_running_folder = QCoreApplication::applicationDirPath().toStdString();
    ConfigureLogging();
    QString browser_settings_error;
    if (!BrowserApplicationsSettings::MergeWithInstalledDefaults(&browser_settings_error)) {
        LogView::AddLog( browser_settings_error );
    }
    utilities::GlobalSetting::tesseract_engine_data_folder = ResolvePath(ReadSetting("TesseractEngineDataFolder").toString());
    utilities::GlobalSetting::tesseract_engine_language = ReadSetting("TesseractEngineLanguage", "eng").toString().toStdString();
    utilities::GlobalSetting::SetImageFileFolder(ResolveWritablePath(ReadSetting("DebugImagesLocation").toString()));
    ConfigureDebugSettings();
    RegisterDefaultActionBindings();
    utilities::Logger::Info("SkipAdClicker startup completed.", "StartUp");
    LogView::AddLog("SkipAdClicker startup completed.");
}

void StartUp::RegisterDefaultActionBindings()
{
    static bool registered = false;
    if (registered) {
        return;
    }

    automationtest::utilities::logViewFuncPtr = LogView::AddLogFromStdString;
    auto& load_functions = utilities::status::LoadFunctions::Instance();
    playtestbook::services::StaticFunctionService::Instance().RegisterDefaultFunctions();
    services::ApplicationService::RegisterBindings(load_functions);
    services::ChromeService::RegisterBindings(load_functions);
    BrowserApplicationsSettings::RegisterBindings(load_functions);
    FullScreenControls::RegisterBindings(load_functions);
    userinterfacelib::SkipAdDetector::RegisterBindings(load_functions);
	OSRelatedFunctions::RegisterBindings(load_functions);
    registered = true;
}

void StartUp::RegisterInteractiveAction(utilities::interface::IInteractiveAction* action_service)
{
    auto& dependency_store = utilities::dependency_injection::DependencyStore::Instance();
    dependency_store.AddType("IInteractiveAction", action_service);
    dependency_store.interactive_action = action_service;
    utilities::GlobalVariables::recent_action_service = std::shared_ptr<utilities::interface::IRecentActionService>(
        &playtestbook::services::RecentActivityService::Instance(),
        [](utilities::interface::IRecentActionService*) {});
}

void StartUp::OpenOneFolder(const std::string& file_path, const std::function<bool()>& cancellation_requested, const std::string& filename)
{
    LogView::AddLog(QString("Start to process %1").arg(QString::fromStdString(file_path)));
    RecentFiles::Instance().AddRecentFile(QString::fromStdString(file_path));
    utilities::GlobalSetting::current_running_folder = file_path;

    auto& variable_service = utilities::services::VariableService::Instance();
    auto& dependency_store = utilities::dependency_injection::DependencyStore::Instance();
    dependency_store.Initialize();
    dependency_store.AddType("VariableService", &variable_service);

    RegisterDefaultActionBindings();
    utilities::status::LoadFunctions::Instance().Initialize();

    const auto setting_folder = utilities::FilePathLib::PathCombine(file_path, "Settings");
    playtestbook::services::StaticResourceService::Instance().LoadObjects(setting_folder, variable_service);
    playtestbook::services::StaticFunctionService::Instance().LoadFunctions(utilities::FilePathLib::PathCombine(file_path, "Functions"));

    auto test_book = std::make_shared<testbooklib::TestBook>(testbooklib::parser::TestBookParser {}.Parse(file_path, filename));
    playtestbook::TestBookPlayer player(test_book);
    player.Play(cancellation_requested);

    LogView::AddLog(QString("Completed to process %1").arg(QString::fromStdString(file_path)));
}

std::optional<std::string> StartUp::StartupFolder()
{
    auto startup = ReadSetting("StartupFolder", "").toString().trimmed();
    if (startup.isEmpty()) {
        return std::nullopt;
    }
    return ResolvePath(startup);
}

bool StartUp::MinimizedWhenStarted()
{
    return QSettings().value(kMinimizedWhenStartedSetting, false).toBool();
}

void StartUp::SetMinimizedWhenStarted(bool minimized)
{
    QSettings().setValue(kMinimizedWhenStartedSetting, minimized);
}

bool StartUp::AutoStartPromptShown()
{
    return QSettings().value(kAutoStartPromptShownSetting, false).toBool();
}

void StartUp::SetAutoStartPromptShown()
{
    QSettings().setValue(kAutoStartPromptShownSetting, true);
}

std::string StartUp::VersionNumber()
{
    return AppSettings().value("VersionNumber", "0.0.0").toString().trimmed().toStdString();
}

QSettings StartUp::AppSettings()
{
    const auto filename = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("SkipAdClicker.ini");
    return QSettings(filename, QSettings::IniFormat);
}

} // namespace automationtest::app
