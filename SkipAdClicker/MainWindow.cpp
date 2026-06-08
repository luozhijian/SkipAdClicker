#include "MainWindow.hpp"

#include "ApplicationAutoStart.hpp"
#include "RecentFiles.hpp"
#include "StartUp.hpp"
#include "../Utilities/Exceptions/TestCancellingException.hpp"
#include "../Utilities/Exceptions/TestFlowStopException.hpp"
#include "../Utilities/GlobalSetting.hpp"
#include "../Utilities/Status/LoadFunctions.hpp"
#include  "SharedMemorySemaphore.hpp"


#include <QApplication>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QCursor>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenuBar>
#include <QMessageBox>
#include <QMetaObject>
#include <QSignalBlocker>
#include <QThread>
#include <QTimer>
#include <QWindow>

#if defined(Q_OS_WIN)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <wtsapi32.h>
#endif

#if defined(__linux__)
#include <QGuiApplication>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#if defined(AUTOMATIOTEST_HAS_QT_DBUS)
#include <QDBusConnection>
#endif
#endif

namespace automationtest::app {

namespace {

bool IsUiThread()
{
    const auto* application = QCoreApplication::instance();
    return application == nullptr || QThread::currentThread() == application->thread();
}

void RefreshWidgetTree(QWidget* widget)
{
    if (!widget) {
        return;
    }
    if (!IsUiThread()) {
        return;
    }

    widget->update();
    if (widget->isVisible()) {
        widget->repaint();
    }

    // Safely find only child widgets, avoiding qobject_cast loops
    const auto childWidgets = widget->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);

    for (auto* child_widget : childWidgets) {
        RefreshWidgetTree(child_widget);
    }
}

void ScheduleWidgetTreeRefresh(QWidget* widget)
{
    if (!widget) {
        return;
    }
    // if (!IsUiThread()) {
    //     QMetaObject::invokeMethod(widget, [widget]() {
    //         ScheduleWidgetTreeRefresh(widget);
    //     }, Qt::QueuedConnection);
    //     return;
    // }

    RefreshWidgetTree(widget);
}

#if defined(__linux__)
int IgnoreX11Error(Display*, XErrorEvent*)
{
    return 0;
}

bool IsX11Platform()
{
    return QGuiApplication::platformName().contains("xcb", Qt::CaseInsensitive);
}

void RequestX11Activation(WId window_id)
{
    if (window_id == 0 || !IsX11Platform()) {
        return;
    }

    Display* display = XOpenDisplay(nullptr);
    if (display == nullptr) {
        return;
    }

    auto* previous_error_handler = XSetErrorHandler(IgnoreX11Error);
    const ::Window window = static_cast<::Window>(window_id);
    const ::Window root = DefaultRootWindow(display);
    const Atom active_window = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);

    XMapRaised(display, window);
    XRaiseWindow(display, window);

    if (active_window != None) {
        XEvent event {};
        event.xclient.type = ClientMessage;
        event.xclient.window = window;
        event.xclient.message_type = active_window;
        event.xclient.format = 32;
        event.xclient.data.l[0] = 2; // Pager-style activation is accepted by more Linux window managers.
        event.xclient.data.l[1] = CurrentTime;
        event.xclient.data.l[2] = 0;
        event.xclient.data.l[3] = 0;
        event.xclient.data.l[4] = 0;

        XSendEvent(display, root, False, SubstructureRedirectMask | SubstructureNotifyMask, &event);
    }

    XWindowAttributes attributes {};
    if (XGetWindowAttributes(display, window, &attributes) != 0 && attributes.map_state == IsViewable) {
        XSetInputFocus(display, window, RevertToParent, CurrentTime);
    }

    XSync(display, False);
    XSetErrorHandler(previous_error_handler);
    XCloseDisplay(display);
}
#endif

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("SkipAdClicker");
    resize(800, 450);

    log_view_ = new LogView(this);
    setCentralWidget(log_view_);

    default_icon_ = QIcon(QCoreApplication::applicationDirPath() + "/SkipAdClicker.ico");
    running_icon_ = QIcon(QCoreApplication::applicationDirPath() + "/SkipAdClicker_ON.ico");
    if (!default_icon_.isNull()) {
        setWindowIcon(default_icon_);
    }

    BuildMenus();
    InitializeTrayIcon();
    UpdateIconsByRunState();

#if defined(Q_OS_WIN)
    RegisterSessionNotifications();
#endif
#if defined(__linux__) && defined(AUTOMATIOTEST_HAS_QT_DBUS)
    RegisterScreenSaverNotifications();
#endif

    auto& load_functions = utilities::status::LoadFunctions::Instance();
    load_functions.RegisterMethod("MinimizedApplication", utilities::status::LoadFunctions::RegisteredMethod {
        .declaring_type = "MainWindow",
        .invoke = [this](const std::vector<std::any>&) -> std::any {
            MinimizedApplication();
            return {};
        },
    });
}

MainWindow::~MainWindow()
{
#if defined(Q_OS_WIN)
    UnregisterSessionNotifications();
#endif
#if defined(__linux__) && defined(AUTOMATIOTEST_HAS_QT_DBUS)
    UnregisterScreenSaverNotifications();
#endif
    StopTest();
    if (worker_thread_ != nullptr) {
        worker_thread_->quit();
        worker_thread_->wait(1000);
    }
}

void MainWindow::SetScreenLockBlockState(bool is_locked)
{
    if (utilities::GlobalSetting::IsScreenLockBlockSet() == is_locked) {
        return;
    }

    if (is_locked) {
        utilities::GlobalSetting::SetScreenLockBlock();
        LogView::AddLog("Screen locked. Pausing cancellation checks.");
    } else {
        utilities::GlobalSetting::ClearScreenLockBlock();
        LogView::AddLog("Screen unlocked. Resuming cancellation checks.");
    }
}

#if defined(Q_OS_WIN)
void MainWindow::RegisterSessionNotifications()
{
    session_notification_window_ = winId();
    if (session_notification_window_ == 0) {
        return;
    }

    session_notifications_registered_ =
        WTSRegisterSessionNotification(reinterpret_cast<HWND>(session_notification_window_), NOTIFY_FOR_THIS_SESSION) != FALSE;
}

void MainWindow::UnregisterSessionNotifications()
{
    if (!session_notifications_registered_ || session_notification_window_ == 0) {
        return;
    }

    WTSUnRegisterSessionNotification(reinterpret_cast<HWND>(session_notification_window_));
    session_notifications_registered_ = false;
    session_notification_window_ = {};
}
#endif

#if defined(__linux__) && defined(AUTOMATIOTEST_HAS_QT_DBUS)
void MainWindow::RegisterScreenSaverNotifications()
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) {
        return;
    }

    bus.connect(
        QStringLiteral("org.freedesktop.ScreenSaver"),
        QStringLiteral("/org/freedesktop/ScreenSaver"),
        QStringLiteral("org.freedesktop.ScreenSaver"),
        QStringLiteral("ActiveChanged"),
        this,
        SLOT(OnScreenSaverActiveChanged(bool)));
    bus.connect(
        QStringLiteral("org.gnome.ScreenSaver"),
        QStringLiteral("/org/gnome/ScreenSaver"),
        QStringLiteral("org.gnome.ScreenSaver"),
        QStringLiteral("ActiveChanged"),
        this,
        SLOT(OnScreenSaverActiveChanged(bool)));
}

void MainWindow::UnregisterScreenSaverNotifications()
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) {
        return;
    }

    bus.disconnect(
        QStringLiteral("org.freedesktop.ScreenSaver"),
        QStringLiteral("/org/freedesktop/ScreenSaver"),
        QStringLiteral("org.freedesktop.ScreenSaver"),
        QStringLiteral("ActiveChanged"),
        this,
        SLOT(OnScreenSaverActiveChanged(bool)));
    bus.disconnect(
        QStringLiteral("org.gnome.ScreenSaver"),
        QStringLiteral("/org/gnome/ScreenSaver"),
        QStringLiteral("org.gnome.ScreenSaver"),
        QStringLiteral("ActiveChanged"),
        this,
        SLOT(OnScreenSaverActiveChanged(bool)));
}

void MainWindow::OnScreenSaverActiveChanged(bool active)
{
    SetScreenLockBlockState(active);
}
#endif

void MainWindow::BuildMenus()
{
    auto* file_menu = menuBar()->addMenu("&File");
    file_menu->addAction("Open", this, [this]() {
        const auto file = QFileDialog::getOpenFileName(this, "Browse Test Files", QString::fromStdString(StartUp::StartupFolder().value_or(".")), "txt files (*.txt)");
        if (file.isEmpty()) {
            return;
        }
        OpenOneFolder(QFileInfo(file).absolutePath());
    });
    file_menu->addAction("Start Last Test", this, [this]() {
        if (last_run_file_path_.isEmpty()) {
            QMessageBox::information(this, "SkipAdClicker", "There is no previous batch run");
            return;
        }
        OpenOneFolder(last_run_file_path_);
    });
    file_menu->addAction("Stop Test", this, [this]() {
        StopTest();
    });
    file_menu->addSeparator();
    recent_tests_menu_ = file_menu->addMenu("Recent Tests");
    PopulateRecentMenu();
    file_menu->addSeparator();
    file_menu->addAction("Exit", this, [this]() {
        StopTest();
        QApplication::quit();
    });

    auto* view_menu = menuBar()->addMenu("&View");
    view_menu->addAction("Log", this, [this]() {
        log_view_->setFocus();
    });

    auto* settings_menu = menuBar()->addMenu("&Settings");
    minimized_when_started_action_ = settings_menu->addAction("Minimized when started");
    minimized_when_started_action_->setCheckable(true);
    minimized_when_started_action_->setChecked(StartUp::MinimizedWhenStarted());
    connect(minimized_when_started_action_, &QAction::toggled, this, [](bool minimized) {
        StartUp::SetMinimizedWhenStarted(minimized);
    });

    start_after_restart_action_ = settings_menu->addAction("Start Application After Restart");
    start_after_restart_action_->setCheckable(true);
    start_after_restart_action_->setChecked(ApplicationAutoStart::IsEnabled());
    connect(start_after_restart_action_, &QAction::toggled, this, [this](bool enabled) {
        SetStartAfterRestartEnabled(enabled);
    });
}

bool MainWindow::SetStartAfterRestartEnabled(bool enabled)
{
    QString error_message;
    if (!ApplicationAutoStart::SetEnabled(enabled, &error_message)) {
        const QSignalBlocker blocker(start_after_restart_action_);
        start_after_restart_action_->setChecked(ApplicationAutoStart::IsEnabled());
        QMessageBox::warning(
            this,
            "SkipAdClicker",
            QString("Could not update the startup setting.\n\n%1").arg(error_message));
        return false;
    }

    StartUp::SetMinimizedWhenStarted(enabled);

    const QSignalBlocker restart_blocker(start_after_restart_action_);
    const QSignalBlocker minimized_blocker(minimized_when_started_action_);
    start_after_restart_action_->setChecked(enabled);
    minimized_when_started_action_->setChecked(enabled);
    return true;
}

void MainWindow::PromptForAutoStartOnFirstRun()
{
    if (StartUp::AutoStartPromptShown()) {
        return;
    }

    StartUp::SetAutoStartPromptShown();
    if (ApplicationAutoStart::IsEnabled()) {
        return;
    }

    const auto answer = QMessageBox::question(
        this,
        "Start SkipAdClicker Automatically?",
        "Would you like SkipAdClicker to start automatically after your computer restarts?\n\n"
        "If you choose Yes, SkipAdClicker will:\n"
        "- start automatically after restart\n"
        "- start minimized in the system tray\n\n"
        "You can change these options later from the Settings menu.",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (answer == QMessageBox::Yes) {
        SetStartAfterRestartEnabled(true);
    }
}

void MainWindow::InitializeTrayIcon()
{
    tray_icon_ = new QSystemTrayIcon(this);
    tray_icon_->setToolTip(windowTitle());
    tray_icon_->setIcon(default_icon_);

    auto* tray_menu = new QMenu(this);
    tray_menu->addAction("Maximize", this, [this]() { ShowAndRestore(); });
    tray_menu->addAction("Exit", this, []() { QApplication::quit(); });
    tray_menu->addSeparator();
    tray_menu->addAction("Start", this, [this]() {
        if (!last_run_file_path_.isEmpty()) {
            OpenOneFolder(last_run_file_path_);
        }
    });
    tray_menu->addAction("Stop", this, [this]() { StopTest(); });
    tray_icon_->setContextMenu(tray_menu);

    connect(tray_icon_, &QSystemTrayIcon::activated, this, [this, tray_menu](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            tray_menu->popup(QCursor::pos());
            return;
        }

        if (reason == QSystemTrayIcon::DoubleClick) {
            ShowAndRestore();
        }
    });

    tray_icon_->show();
}

void MainWindow::PopulateRecentMenu()
{
    recent_tests_menu_->clear();
    for (const auto& path : RecentFiles::Instance().RecentFileList()) {
        if (path.trimmed().isEmpty()) {
            continue;
        }
        recent_tests_menu_->addAction(path, this, [this, path]() {
            OpenOneFolder(path);
        });
    }
}

void MainWindow::OpenOneFolder(const QString& file_path)
{
    if (call_is_in_process_) {
        QMessageBox::information(this, "SkipAdCliker", "There is already a process running, please stop it first");
        return;
    }

    last_run_file_path_ = file_path;
    RunFolderInBackground(file_path);
}

void MainWindow::RunFolderInBackground(const QString& file_path)
{
    cancellation_requested_.store(false);
    utilities::GlobalSetting::ClearStopTest();
    SetCallInProcess(true);
    LogView::AddLog(QString("run %1").arg(file_path));

    worker_thread_ = QThread::create([this, file_path]() {
        try {
            StartUp::OpenOneFolder(file_path.toStdString(), [this]() {
                return cancellation_requested_.load();
            });
        } catch (const utilities::exceptions::TestCancellingException&) {
            LogView::AddLog("Batch Run stopped");
        } catch (const std::exception& exception) {
            LogView::AddLog(QString("Error: %1").arg(exception.what()));
        }

        QMetaObject::invokeMethod(this, [this, file_path]() {
            SetCallInProcess(false);
            LogView::AddLog(QString("completed run %1").arg(file_path));
            if (worker_thread_ != nullptr) {
                worker_thread_->deleteLater();
                worker_thread_ = nullptr;
            }
            PopulateRecentMenu();
        }, Qt::QueuedConnection);
    });
    worker_thread_->start();
}

void MainWindow::SetCallInProcess(bool value)
{
    if (!IsUiThread()) {
        QMetaObject::invokeMethod(this, [this, value]() {
            SetCallInProcess(value);
        }, Qt::QueuedConnection);
        return;
    }

    call_is_in_process_ = value;
    UpdateIconsByRunState();
}

void MainWindow::UpdateIconsByRunState()
{
    if (!IsUiThread()) {
        QMetaObject::invokeMethod(this, [this]() {
            UpdateIconsByRunState();
        }, Qt::QueuedConnection);
        return;
    }

    const auto icon = call_is_in_process_ && !running_icon_.isNull() ? running_icon_ : default_icon_;
    if (!icon.isNull()) {
        setWindowIcon(icon);
        if (tray_icon_ != nullptr) {
            tray_icon_->setIcon(icon);
        }
    }
    if (tray_icon_ != nullptr) {
        tray_icon_->setToolTip(call_is_in_process_ ? "SkipAdClicker - Running..." : "SkipAdClicker - Idle");
    }
}

void MainWindow::StopTest()
{
    if (!call_is_in_process_) {
        return;
    }

    utilities::GlobalSetting::StopTest();
    cancellation_requested_.store(true);
    LogView::AddLog("Stop running requested...");
}

void MainWindow::ScheduleFocusRepaint()
{
    if (!IsUiThread()) {
        QMetaObject::invokeMethod(this, [this]() {
            ScheduleFocusRepaint();
        }, Qt::QueuedConnection);
        return;
    }

    setWindowState(Qt::WindowActive) ;
    ScheduleWidgetTreeRefresh(this);

    // for (const int delay_ms : { 300 }) {
    //     QTimer::singleShot(delay_ms, this, [this]() {
    //         if (!isVisible()) {
    //             return;
    //         }
    //         ScheduleWidgetTreeRefresh(this);
    //     });
    // }
}

void MainWindow::ShowAndRestore()
{
    if (!IsUiThread()) {
        QMetaObject::invokeMethod(this, [this]() {
            ShowAndRestore();
        }, Qt::QueuedConnection);
        return;
    }

    if (tray_icon_ != nullptr && !tray_icon_->isVisible()) {
        tray_icon_->show();
    }

    const bool restore_maximized = isMaximized();
 
 
    show();
    if (restore_maximized) {
        showMaximized();
    } else {
 
    showNormal();
    }

    activateWindow();
    raise();

    setWindowState(Qt::WindowActive) ;

#if defined(__linux__)
    RequestX11Activation(winId());
#endif
    ScheduleFocusRepaint();

    this->showNormal();
    this->raise();
    this->activateWindow();

   setWindowState(Qt::WindowActive) ;

#if defined(__linux__)
    QTimer::singleShot(200, this, [this]() {
        showNormal();
        raise();
        activateWindow();
        setWindowState(Qt::WindowActive) ;
        RequestX11Activation(winId());
        ScheduleFocusRepaint();
        this->showNormal();
        this->raise();
        this->activateWindow();
    });
#endif
}

void MainWindow::ReStart()
{
    if (!last_run_file_path_.isEmpty() && !call_is_in_process_) {
        OpenOneFolder(last_run_file_path_);
    }
}

void MainWindow::ProcessCommandLine()
{
    QThread::msleep(500);

    if ( g_SharedMemorySemaphore->Exists ())
    {
        g_SharedMemorySemaphore->Clear();
    }
    g_SharedMemorySemaphore->CreateForCreator();
    g_SharedMemorySemaphore->Wait(this);

    PromptForAutoStartOnFirstRun();
 
    const auto startup_folder = StartUp::StartupFolder();
    if (startup_folder.has_value() && QDir(QString::fromStdString(*startup_folder)).exists()) {
        OpenOneFolder(QString::fromStdString(*startup_folder));
    }
}

void MainWindow::MinimizedApplication()
{
    if (!IsUiThread()) {
        QMetaObject::invokeMethod(this, [this]() {
            MinimizedApplication();
        }, Qt::QueuedConnection);
        return;
    }

    setWindowState(Qt::WindowMinimized);
}

#if defined(Q_OS_WIN)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
bool MainWindow::nativeEvent(const QByteArray& eventType, void* message, qintptr* result)
#else
bool MainWindow::nativeEvent(const QByteArray& eventType, void* message, long* result)
#endif
{
    Q_UNUSED(eventType)
    Q_UNUSED(result)

    const auto* msg = static_cast<MSG*>(message);
    if (msg == nullptr || msg->message != WM_WTSSESSION_CHANGE) {
        return false;
    }

    switch (msg->wParam) {
    case WTS_SESSION_LOCK:
        SetScreenLockBlockState(true);
        break;
    case WTS_SESSION_UNLOCK:
        SetScreenLockBlockState(false);
        break;
    default:
        break;
    }

    return false;
}
#endif

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (tray_icon_ != nullptr && tray_icon_->isVisible()) {
        hide();
        event->ignore();
        return;
    }
    QMainWindow::closeEvent(event);
}

void MainWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::WindowStateChange && isMinimized() && tray_icon_ != nullptr) {
        hide();
        tray_icon_->show();
    } else if (event->type() == QEvent::ActivationChange && isActiveWindow()) {
        ScheduleFocusRepaint();
    }
    QMainWindow::changeEvent(event);
}

} // namespace automationtest::app
