#ifndef AUTOMATIOTEST_APP_MAINWINDOW_HPP
#define AUTOMATIOTEST_APP_MAINWINDOW_HPP

#include "LogView.hpp"

#include <QAction>
#include <QAtomicInteger>
#include <QMainWindow>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QThread>
#include <QTimer>

#include <atomic>

namespace automationtest::app {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    void ReStart();
    void ProcessCommandLine();
    void MinimizedApplication();
    void ShowAndRestore();
    void PromptForAutoStartOnFirstRun();

protected:
    void closeEvent(QCloseEvent* event) override;
    void changeEvent(QEvent* event) override;
#if defined(Q_OS_WIN)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;
#else
    bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
#endif
#endif

#if defined(__linux__) && defined(AUTOMATIOTEST_HAS_QT_DBUS)
private slots:
    void OnScreenSaverActiveChanged(bool active);
#endif

private:
    static QIcon loadIcoFileSafely(const QString& file_path);
    void InitializeTrayIcon();
    void EnsureTrayIconVisible();
    void HideToTray();
    void ExitApplication();
    void BuildMenus();
    void PopulateRecentMenu();
    void OpenOneFolder(const QString& file_path);
    void RunFolderInBackground(const QString& file_path);
    void SetCallInProcess(bool value);
    void UpdateIconsByRunState();
    void StopTest();
    void ScheduleFocusRepaint();
    void SetScreenLockBlockState(bool is_locked);
    bool IsStartAfterRestartEnabled() const;
    bool SetStartAfterRestartEnabled(bool enabled);
#if defined(Q_OS_WIN)
    void RegisterSessionNotifications();
    void UnregisterSessionNotifications();
#endif
#if defined(__linux__) && defined(AUTOMATIOTEST_HAS_QT_DBUS)
    void RegisterScreenSaverNotifications();
    void UnregisterScreenSaverNotifications();
#endif
    LogView* log_view_ {nullptr};
    QMenu* recent_tests_menu_ {nullptr};
    QAction* minimized_when_started_action_ {nullptr};
    QAction* start_after_restart_action_ {nullptr};
    QSystemTrayIcon* tray_icon_ {nullptr};
    QTimer* tray_retry_timer_ {nullptr};
    QIcon default_icon_ {};
    QIcon running_icon_ {};
    std::atomic_bool cancellation_requested_ {false};
    QThread* worker_thread_ {nullptr};
    bool call_is_in_process_ {false};
    bool exit_requested_ {false};
    QString last_run_file_path_ {};
#if defined(Q_OS_WIN)
    WId session_notification_window_ {};
    bool session_notifications_registered_ {false};
    unsigned int taskbar_created_message_ {0};
#endif
};

} // namespace automationtest::app

#endif // AUTOMATIOTEST_APP_MAINWINDOW_HPP
