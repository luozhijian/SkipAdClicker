#ifndef AUTOMATIOTEST_APP_MAINWINDOW_HPP
#define AUTOMATIOTEST_APP_MAINWINDOW_HPP

#include "LogView.hpp"

#include <QAction>
#include <QAtomicInteger>
#include <QMainWindow>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QThread>

#include <atomic>

namespace automationtest::app {

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    void ReStart();
    void ProcessCommandLine();
    void MinimizedApplication();

protected:
    void closeEvent(QCloseEvent* event) override;
    void changeEvent(QEvent* event) override;

private:
    void InitializeTrayIcon();
    void BuildMenus();
    void PopulateRecentMenu();
    void OpenOneFolder(const QString& file_path);
    void RunFolderInBackground(const QString& file_path);
    void SetCallInProcess(bool value);
    void UpdateIconsByRunState();
    void StopTest();
    void ShowAndRestore();

    LogView* log_view_ {nullptr};
    QMenu* recent_tests_menu_ {nullptr};
    QSystemTrayIcon* tray_icon_ {nullptr};
    QIcon default_icon_ {};
    QIcon running_icon_ {};
    std::atomic_bool cancellation_requested_ {false};
    QThread* worker_thread_ {nullptr};
    bool call_is_in_process_ {false};
    QString last_run_file_path_ {};
};

} // namespace automationtest::app

#endif // AUTOMATIOTEST_APP_MAINWINDOW_HPP
