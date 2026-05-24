#include "MainWindow.hpp"

#include "RecentFiles.hpp"
#include "StartUp.hpp"
#include "../Utilities/Exceptions/TestCancellingException.hpp"
#include "../Utilities/Exceptions/TestFlowStopException.hpp"
#include "../Utilities/GlobalSetting.hpp"
#include "../Utilities/Status/LoadFunctions.hpp"

#include <QApplication>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenuBar>
#include <QMessageBox>
#include <QMetaObject>

namespace automationtest::app {

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
    StopTest();
    if (worker_thread_ != nullptr) {
        worker_thread_->quit();
        worker_thread_->wait(1000);
    }
}

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

    connect(tray_icon_, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick) {
            ShowAndRestore();
        }
    });
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
    call_is_in_process_ = value;
    UpdateIconsByRunState();
}

void MainWindow::UpdateIconsByRunState()
{
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

void MainWindow::ShowAndRestore()
{
    if (tray_icon_ != nullptr) {
        tray_icon_->hide();
    }
    show();
    setWindowState(windowState() & ~Qt::WindowMinimized);
    raise();
    activateWindow();
}

void MainWindow::ReStart()
{
    if (!last_run_file_path_.isEmpty() && !call_is_in_process_) {
        OpenOneFolder(last_run_file_path_);
    }
}

void MainWindow::ProcessCommandLine()
{
    const auto startup_folder = StartUp::StartupFolder();
    if (startup_folder.has_value() && QDir(QString::fromStdString(*startup_folder)).exists()) {
        OpenOneFolder(QString::fromStdString(*startup_folder));
    }
}

void MainWindow::MinimizedApplication()
{
    setWindowState(Qt::WindowMinimized);
}

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
    }
    QMainWindow::changeEvent(event);
}

} // namespace automationtest::app
