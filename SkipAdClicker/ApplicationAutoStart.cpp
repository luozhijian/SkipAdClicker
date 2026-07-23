#include "ApplicationAutoStart.hpp"
#include "LogView.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QStandardPaths>
#include <QString>

#include <string>
#include <vector>

#if defined(Q_OS_WIN)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <appmodel.h>
#include <sddl.h>
#include <taskschd.h>
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
constexpr auto kWindowsTaskName = L"SkipAdClicker Start At Logon";
constexpr auto kPackagedApplicationId = L"App";
constexpr auto kLegacyWindowsRunKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
constexpr auto kLegacyWindowsRunValue = L"SkipAdClicker";
constexpr auto kWindowsTaskSchedulerRightsMessage =
    "Could not update the per-user Windows startup task. Make sure Task Scheduler is enabled "
    "and this task belongs to your current Windows user.";

QString WindowsStartupCommand()
{
    const auto executable = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    if (executable.isEmpty()) {
        return {};
    }
    return QString("\"%1\"").arg(executable);
}

QString WindowsHResultMessage(HRESULT status)
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
        : QString("Windows error 0x%1").arg(static_cast<unsigned long>(status), 8, 16, QChar('0'));
    if (message != nullptr) {
        LocalFree(message);
    }
    return result;
}

bool FailWindowsTask(QString* error_message, const QString& details)
{
    LogView::AddLog(details);
    return Fail(
        error_message,
        QString("%1\n\n%2").arg(kWindowsTaskSchedulerRightsMessage, details));
}

template <typename T>
class ComPtr {
public:
    ~ComPtr()
    {
        Reset();
    }

    T* operator->() const
    {
        return value_;
    }

    T* Get() const
    {
        return value_;
    }

    T** Put()
    {
        Reset();
        return &value_;
    }

    void Reset()
    {
        if (value_ != nullptr) {
            value_->Release();
            value_ = nullptr;
        }
    }

private:
    T* value_ = nullptr;
};

class ScopedBstr {
public:
    explicit ScopedBstr(const wchar_t* value)
        : value_(SysAllocString(value))
    {
    }

    explicit ScopedBstr(const QString& value)
        : value_(SysAllocString(reinterpret_cast<const OLECHAR*>(value.utf16())))
    {
    }

    ~ScopedBstr()
    {
        SysFreeString(value_);
    }

    BSTR Get() const
    {
        return value_;
    }

private:
    BSTR value_ = nullptr;
};

class ScopedVariant {
public:
    ScopedVariant()
    {
        VariantInit(&value_);
    }

    ~ScopedVariant()
    {
        VariantClear(&value_);
    }

    VARIANT& Get()
    {
        return value_;
    }

private:
    VARIANT value_;
};

class ScopedComInitialize {
public:
    HRESULT Initialize()
    {
        status_ = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        initialized_ = SUCCEEDED(status_);
        if (status_ == RPC_E_CHANGED_MODE) {
            status_ = S_OK;
            initialized_ = false;
        }
        return status_;
    }

    ~ScopedComInitialize()
    {
        if (initialized_) {
            CoUninitialize();
        }
    }

private:
    HRESULT status_ = S_OK;
    bool initialized_ = false;
};

bool IsTaskMissing(HRESULT status)
{
    return status == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)
        || status == HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
}

QString CurrentUserSid()
{
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        return {};
    }

    DWORD token_user_size = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &token_user_size);
    std::vector<BYTE> token_user_buffer(token_user_size);
    if (!GetTokenInformation(token, TokenUser, token_user_buffer.data(), token_user_size, &token_user_size)) {
        CloseHandle(token);
        return {};
    }
    CloseHandle(token);

    auto* token_user = reinterpret_cast<TOKEN_USER*>(token_user_buffer.data());
    wchar_t* sid_text = nullptr;
    if (!ConvertSidToStringSidW(token_user->User.Sid, &sid_text)) {
        return {};
    }

    const auto result = QString::fromWCharArray(sid_text);
    LocalFree(sid_text);
    return result;
}

QString CurrentPackageFamilyName()
{
    UINT32 length = 0;
    auto status = GetCurrentPackageFamilyName(&length, nullptr);
    if (status != ERROR_INSUFFICIENT_BUFFER || length == 0) {
        return {};
    }

    std::wstring package_family_name(length, L'\0');
    status = GetCurrentPackageFamilyName(&length, package_family_name.data());
    if (status != ERROR_SUCCESS) {
        return {};
    }
    if (!package_family_name.empty() && package_family_name.back() == L'\0') {
        package_family_name.pop_back();
    }
    return QString::fromStdWString(package_family_name);
}

QString WindowsExplorerPath()
{
    std::vector<wchar_t> windows_directory(MAX_PATH);
    const auto length = GetWindowsDirectoryW(windows_directory.data(), static_cast<UINT>(windows_directory.size()));
    if (length == 0 || length >= windows_directory.size()) {
        return {};
    }
    return QDir(QString::fromWCharArray(windows_directory.data(), static_cast<qsizetype>(length))).absoluteFilePath("explorer.exe");
}

struct TaskActionCommand {
    QString path;
    QString arguments;
    QString working_directory;
};

TaskActionCommand WindowsTaskActionCommand()
{
    const auto package_family_name = CurrentPackageFamilyName();
    if (!package_family_name.isEmpty()) {
        return TaskActionCommand {
            QDir::toNativeSeparators(WindowsExplorerPath()),
            QString("shell:AppsFolder\\%1!%2").arg(package_family_name, QString::fromWCharArray(kPackagedApplicationId)),
            {}
        };
    }

    return TaskActionCommand {
        QDir::toNativeSeparators(QCoreApplication::applicationFilePath()),
        {},
        QDir::toNativeSeparators(QCoreApplication::applicationDirPath())
    };
}

bool ConnectToTaskScheduler(ComPtr<ITaskService>& service, ComPtr<ITaskFolder>& root_folder, QString* error_message)
{
    auto hr = CoCreateInstance(
        CLSID_TaskScheduler,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_ITaskService,
        reinterpret_cast<void**>(service.Put()));
    if (FAILED(hr)) {
        return FailWindowsTask(
            error_message,
            QString("Could not create Task Scheduler service: %1").arg(WindowsHResultMessage(hr)));
    }

    ScopedVariant empty;
    hr = service->Connect(empty.Get(), empty.Get(), empty.Get(), empty.Get());
    if (FAILED(hr)) {
        return FailWindowsTask(
            error_message,
            QString("Could not connect to Task Scheduler: %1").arg(WindowsHResultMessage(hr)));
    }

    ScopedBstr root_path(L"\\");
    hr = service->GetFolder(root_path.Get(), root_folder.Put());
    if (FAILED(hr)) {
        return FailWindowsTask(
            error_message,
            QString("Could not open the root Task Scheduler folder: %1").arg(WindowsHResultMessage(hr)));
    }

    return true;
}

bool RemoveLegacyRunRegistration(QString* error_message)
{
    HKEY run_key = nullptr;
    const auto open_status = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        kLegacyWindowsRunKey,
        0,
        KEY_SET_VALUE,
        &run_key);
    if (open_status == ERROR_FILE_NOT_FOUND) {
        return true;
    }
    if (open_status != ERROR_SUCCESS) {
        return FailWindowsTask(
            error_message,
            QString("Could not open the old Windows Run registry key for cleanup: %1").arg(open_status));
    }

    auto delete_status = RegDeleteValueW(run_key, kLegacyWindowsRunValue);
    if (delete_status == ERROR_FILE_NOT_FOUND) {
        delete_status = ERROR_SUCCESS;
    }
    RegCloseKey(run_key);

    if (delete_status != ERROR_SUCCESS) {
        return FailWindowsTask(
            error_message,
            QString("Could not remove the old Windows Run registry value: %1").arg(delete_status));
    }
    return true;
}

QString LegacyRunRegistrationCommand()
{
    HKEY run_key = nullptr;
    const auto open_status = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        kLegacyWindowsRunKey,
        0,
        KEY_QUERY_VALUE,
        &run_key);
    if (open_status != ERROR_SUCCESS) {
        return {};
    }

    DWORD value_type = 0;
    DWORD value_size = 0;
    auto query_status = RegQueryValueExW(
        run_key,
        kLegacyWindowsRunValue,
        nullptr,
        &value_type,
        nullptr,
        &value_size);
    std::vector<wchar_t> stored_value;
    if (query_status == ERROR_SUCCESS && value_type == REG_SZ && value_size > 0) {
        stored_value.resize((value_size / sizeof(wchar_t)) + 1);
        query_status = RegQueryValueExW(
            run_key,
            kLegacyWindowsRunValue,
            nullptr,
            &value_type,
            reinterpret_cast<BYTE*>(stored_value.data()),
            &value_size);
    }
    RegCloseKey(run_key);

    return query_status == ERROR_SUCCESS
        && value_type == REG_SZ
        && !stored_value.empty()
        ? QString::fromWCharArray(stored_value.data())
        : QString {};
}

bool IsLegacyRunRegistrationEnabled()
{
    return !LegacyRunRegistrationCommand().isEmpty();
}

bool IsLegacyRunRegistrationCurrent()
{
    return LegacyRunRegistrationCommand() == WindowsStartupCommand();
}

bool IsPlatformRegistrationEnabled()
{
    ScopedComInitialize com;
    const auto initialize_status = com.Initialize();
    if (FAILED(initialize_status)) {
        return IsLegacyRunRegistrationEnabled();
    }

    ComPtr<ITaskService> service;
    ComPtr<ITaskFolder> root_folder;
    if (!ConnectToTaskScheduler(service, root_folder, nullptr)) {
        return IsLegacyRunRegistrationEnabled();
    }

    ComPtr<IRegisteredTask> registered_task;
    ScopedBstr task_name(kWindowsTaskName);
    const auto hr = root_folder->GetTask(task_name.Get(), registered_task.Put());
    if (IsTaskMissing(hr)) {
        return IsLegacyRunRegistrationEnabled();
    }
    if (FAILED(hr)) {
        return IsLegacyRunRegistrationEnabled();
    }

    VARIANT_BOOL enabled = VARIANT_FALSE;
    if (FAILED(registered_task->get_Enabled(&enabled))) {
        return false;
    }
    return enabled == VARIANT_TRUE || IsLegacyRunRegistrationEnabled();
}

bool IsPlatformRegistrationCurrent()
{
    ScopedComInitialize com;
    if (FAILED(com.Initialize())) {
        return IsLegacyRunRegistrationCurrent();
    }

    ComPtr<ITaskService> service;
    ComPtr<ITaskFolder> root_folder;
    if (!ConnectToTaskScheduler(service, root_folder, nullptr)) {
        return IsLegacyRunRegistrationCurrent();
    }

    ComPtr<IRegisteredTask> registered_task;
    ScopedBstr task_name(kWindowsTaskName);
    if (FAILED(root_folder->GetTask(task_name.Get(), registered_task.Put()))) {
        return IsLegacyRunRegistrationCurrent();
    }

    ComPtr<ITaskDefinition> task_definition;
    if (FAILED(registered_task->get_Definition(task_definition.Put()))) {
        return false;
    }
    ComPtr<IActionCollection> actions;
    if (FAILED(task_definition->get_Actions(actions.Put()))) {
        return false;
    }
    ComPtr<IAction> action;
    if (FAILED(actions->get_Item(1, action.Put()))) {
        return false;
    }
    ComPtr<IExecAction> exec_action;
    if (FAILED(action->QueryInterface(IID_PPV_ARGS(exec_action.Put())))) {
        return false;
    }

    BSTR stored_path = nullptr;
    BSTR stored_arguments = nullptr;
    if (FAILED(exec_action->get_Path(&stored_path))
        || FAILED(exec_action->get_Arguments(&stored_arguments))) {
        SysFreeString(stored_path);
        SysFreeString(stored_arguments);
        return false;
    }

    const auto expected = WindowsTaskActionCommand();
    const auto path_matches =
        QString::fromWCharArray(stored_path).compare(expected.path, Qt::CaseInsensitive) == 0;
    const auto arguments_match =
        QString::fromWCharArray(stored_arguments).compare(expected.arguments, Qt::CaseInsensitive) == 0;
    SysFreeString(stored_path);
    SysFreeString(stored_arguments);
    return path_matches && arguments_match;
}

bool UpdatePlatformRegistration(bool enabled, QString* error_message)
{
    ScopedComInitialize com;
    const auto initialize_status = com.Initialize();
    if (FAILED(initialize_status)) {
        return FailWindowsTask(
            error_message,
            QString("Could not initialize COM for Task Scheduler: %1").arg(WindowsHResultMessage(initialize_status)));
    }

    ComPtr<ITaskService> service;
    ComPtr<ITaskFolder> root_folder;
    if (!ConnectToTaskScheduler(service, root_folder, error_message)) {
        return false;
    }

    ScopedBstr task_name(kWindowsTaskName);
    if (!enabled) {
        ComPtr<IRegisteredTask> registered_task;
        const auto get_status = root_folder->GetTask(task_name.Get(), registered_task.Put());
        if (!IsTaskMissing(get_status) && FAILED(get_status)) {
            return FailWindowsTask(
                error_message,
                QString("Could not open startup task '%1': %2")
                    .arg(QString::fromWCharArray(kWindowsTaskName), WindowsHResultMessage(get_status)));
        }

        if (registered_task.Get() != nullptr) {
            const auto disable_status = registered_task->put_Enabled(VARIANT_FALSE);
            if (FAILED(disable_status)) {
                return FailWindowsTask(
                    error_message,
                    QString("Could not disable startup task '%1': %2")
                        .arg(QString::fromWCharArray(kWindowsTaskName), WindowsHResultMessage(disable_status)));
            }
        }

        if (!RemoveLegacyRunRegistration(error_message)) {
            return false;
        }
        LogView::AddLog(QString("Run at Startup cleared: ") + kApplicationName);
        return true;
    }

    const auto action_command = WindowsTaskActionCommand();
    if (action_command.path.isEmpty()) {
        return Fail(error_message, "Could not determine the Windows startup task command.");
    }

    ComPtr<ITaskDefinition> task_definition;
    auto hr = service->NewTask(0, task_definition.Put());
    if (FAILED(hr)) {
        return FailWindowsTask(
            error_message,
            QString("Could not create startup task definition: %1").arg(WindowsHResultMessage(hr)));
    }

    ComPtr<IRegistrationInfo> registration_info;
    hr = task_definition->get_RegistrationInfo(registration_info.Put());
    if (FAILED(hr)) {
        return FailWindowsTask(
            error_message,
            QString("Could not get startup task registration info: %1").arg(WindowsHResultMessage(hr)));
    }
    ScopedBstr author(kApplicationName);
    ScopedBstr description(L"Starts SkipAdClicker when the current user logs on.");
    registration_info->put_Author(author.Get());
    registration_info->put_Description(description.Get());

    ComPtr<IPrincipal> principal;
    hr = task_definition->get_Principal(principal.Put());
    if (FAILED(hr)) {
        return FailWindowsTask(
            error_message,
            QString("Could not get startup task principal: %1").arg(WindowsHResultMessage(hr)));
    }
    const auto user_sid = CurrentUserSid();
    if (!user_sid.isEmpty()) {
        ScopedBstr user_id(user_sid);
        principal->put_UserId(user_id.Get());
    }
    principal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);
    principal->put_RunLevel(TASK_RUNLEVEL_LUA);

    ComPtr<ITaskSettings> settings;
    hr = task_definition->get_Settings(settings.Put());
    if (FAILED(hr)) {
        return FailWindowsTask(
            error_message,
            QString("Could not get startup task settings: %1").arg(WindowsHResultMessage(hr)));
    }
    settings->put_Enabled(VARIANT_TRUE);
    settings->put_StartWhenAvailable(VARIANT_TRUE);
    settings->put_DisallowStartIfOnBatteries(VARIANT_FALSE);
    settings->put_StopIfGoingOnBatteries(VARIANT_FALSE);
    settings->put_MultipleInstances(TASK_INSTANCES_IGNORE_NEW);

    ComPtr<ITriggerCollection> triggers;
    hr = task_definition->get_Triggers(triggers.Put());
    if (FAILED(hr)) {
        return FailWindowsTask(
            error_message,
            QString("Could not get startup task triggers: %1").arg(WindowsHResultMessage(hr)));
    }

    ComPtr<ITrigger> trigger;
    hr = triggers->Create(TASK_TRIGGER_LOGON, trigger.Put());
    if (FAILED(hr)) {
        return FailWindowsTask(
            error_message,
            QString("Could not create startup logon trigger: %1").arg(WindowsHResultMessage(hr)));
    }

    ComPtr<ILogonTrigger> logon_trigger;
    hr = trigger->QueryInterface(IID_PPV_ARGS(logon_trigger.Put()));
    if (FAILED(hr)) {
        return FailWindowsTask(
            error_message,
            QString("Could not configure startup logon trigger: %1").arg(WindowsHResultMessage(hr)));
    }
    ScopedBstr trigger_id(L"CurrentUserLogon");
    logon_trigger->put_Id(trigger_id.Get());
    if (!user_sid.isEmpty()) {
        ScopedBstr user_id(user_sid);
        logon_trigger->put_UserId(user_id.Get());
    }

    ComPtr<IActionCollection> actions;
    hr = task_definition->get_Actions(actions.Put());
    if (FAILED(hr)) {
        return FailWindowsTask(
            error_message,
            QString("Could not get startup task actions: %1").arg(WindowsHResultMessage(hr)));
    }

    ComPtr<IAction> action;
    hr = actions->Create(TASK_ACTION_EXEC, action.Put());
    if (FAILED(hr)) {
        return FailWindowsTask(
            error_message,
            QString("Could not create startup task action: %1").arg(WindowsHResultMessage(hr)));
    }

    ComPtr<IExecAction> exec_action;
    hr = action->QueryInterface(IID_PPV_ARGS(exec_action.Put()));
    if (FAILED(hr)) {
        return FailWindowsTask(
            error_message,
            QString("Could not configure startup task action: %1").arg(WindowsHResultMessage(hr)));
    }
    ScopedBstr action_path(action_command.path);
    exec_action->put_Path(action_path.Get());
    if (!action_command.arguments.isEmpty()) {
        ScopedBstr arguments(action_command.arguments);
        exec_action->put_Arguments(arguments.Get());
    }
    if (!action_command.working_directory.isEmpty()) {
        ScopedBstr working_directory(action_command.working_directory);
        exec_action->put_WorkingDirectory(working_directory.Get());
    }

    ScopedVariant empty;
    ComPtr<IRegisteredTask> registered_task;
    hr = root_folder->RegisterTaskDefinition(
        task_name.Get(),
        task_definition.Get(),
        TASK_CREATE_OR_UPDATE,
        empty.Get(),
        empty.Get(),
        TASK_LOGON_INTERACTIVE_TOKEN,
        empty.Get(),
        registered_task.Put());
    if (FAILED(hr)) {
        return FailWindowsTask(
            error_message,
            QString("Could not register startup task '%1': %2")
                .arg(QString::fromWCharArray(kWindowsTaskName), WindowsHResultMessage(hr)));
    }

    hr = registered_task->put_Enabled(VARIANT_TRUE);
    if (FAILED(hr)) {
        return FailWindowsTask(
            error_message,
            QString("Could not enable startup task '%1': %2")
                .arg(QString::fromWCharArray(kWindowsTaskName), WindowsHResultMessage(hr)));
    }

    if (!RemoveLegacyRunRegistration(error_message)) {
        return false;
    }

    if (action_command.arguments.isEmpty()) {
        LogView::AddLog(QString("Run at Startup task: ") + kApplicationName + QString(" ") + action_command.path);
    } else {
        LogView::AddLog(QString("Run at Startup task: ") + kApplicationName + QString(" ") + action_command.path + QString(" ") + action_command.arguments);
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

bool IsPlatformRegistrationCurrent()
{
    QFile file(LaunchAgentPath());
    return file.open(QIODevice::ReadOnly) && file.readAll() == LaunchAgentContents();
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

bool IsPlatformRegistrationCurrent()
{
    QFile file(LinuxAutoStartPath());
    return file.open(QIODevice::ReadOnly) && file.readAll() == LinuxDesktopEntry();
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

bool IsPlatformRegistrationCurrent()
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

bool ApplicationAutoStart::UsesCurrentApplication()
{
    return IsPlatformRegistrationCurrent();
}

bool ApplicationAutoStart::SetEnabled(bool enabled, QString* error_message)
{
    return UpdatePlatformRegistration(enabled, error_message);
}

} // namespace automationtest::app
