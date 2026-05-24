#include <thread>
#include "LogView.hpp"

#include <QApplication>
#include <QScreen>
#include <QPixmap>
#include <QGuiApplication>
#include <QClipboard>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QMetaObject>
#include <QPushButton>
#include <QStandardItem>
#include <QThread>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace automationtest::app {
namespace {

QList<QPixmap> CaptureScreenshots(const QList<QScreen*>& screens)
{
    QList<QPixmap> screenshots;
    for (auto* screen : screens) {
        if (screen == nullptr) {
            continue;
        }

        const QPixmap screenshot = screen->grabWindow(0);
        if (!screenshot.isNull()) {
            screenshots.push_back(screenshot);
        }
    }
    return screenshots;
}

int CopyScreenshotsToClipboard(QWidget* parent, const QList<QPixmap>& screenshots)
{
    auto* clipboard = QApplication::clipboard();
    if (clipboard == nullptr || screenshots.empty()) {
        return 0;
    }

    int copied_count = 0;
    for (int index = 0; index < screenshots.size(); ++index) {
        clipboard->setPixmap(screenshots[index], QClipboard::Clipboard);
        ++copied_count;

        if (screenshots.size() > 1 && index + 1 < screenshots.size()) {
            QMessageBox::information(
                parent,
                "Screenshot copied",
                QString("Screenshot %1 of %2 is now on the clipboard. Paste it, then click OK to copy the next monitor screenshot.")
                    .arg(index + 1)
                    .arg(screenshots.size()));
        }
    }

    return copied_count;
}

} // namespace

QStandardItemModel* LogView::model_ = nullptr;

LogFilterModel::LogFilterModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}

void LogFilterModel::SetPattern(const QString& pattern)
{
    pattern_ = pattern.trimmed();
    regex_ = QRegularExpression(pattern_);
    invalidateFilter();
}

bool LogFilterModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    if (pattern_.isEmpty() || !regex_.isValid()) {
        return true;
    }

    const auto message_index = sourceModel()->index(source_row, 1, source_parent);
    return regex_.match(sourceModel()->data(message_index).toString()).hasMatch();
}

LogView::LogView(QWidget* parent)
    : QWidget(parent)
{
    EnsureModel();

    auto* root = new QVBoxLayout(this);
    auto* filter_row = new QHBoxLayout();
    filter_edit_ = new QLineEdit(this);
    filter_edit_->setPlaceholderText("Filter (regex)");
    auto* clear_button = new QPushButton("Clear", this);
    auto* clear_log_button = new QPushButton("Clear log", this);
    auto* report_error_button = new QPushButton("Report error", this);
    filter_row->addWidget(filter_edit_);
    filter_row->addWidget(clear_button);
    filter_row->addWidget(clear_log_button);
    filter_row->addWidget(report_error_button);

    table_ = new QTableView(this);
    filter_model_ = new LogFilterModel(this);
    filter_model_->setSourceModel(model_);
    table_->setModel(filter_model_);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->verticalHeader()->hide();
    table_->setAlternatingRowColors(true);

    root->addLayout(filter_row);
    root->addWidget(table_);

    connect(filter_edit_, &QLineEdit::textChanged, this, [this](const QString& text) {
        filter_model_->SetPattern(text);
    });
    connect(clear_button, &QPushButton::clicked, this, [this]() {
        filter_edit_->clear();
    });
    connect(clear_log_button, &QPushButton::clicked, this, [this]() {
        EnsureModel();
        model_->removeRows(0, model_->rowCount());
        filter_model_->invalidate();
    });
    connect(report_error_button, &QPushButton::clicked, this, [this]() {
        QWidget* window_to_hide = window();
        if (window_to_hide == nullptr) {
            window_to_hide = this;
        }

        const bool was_visible = window_to_hide->isVisible();
        window_to_hide->hide();
        QApplication::processEvents();
        QThread::msleep(100);
        QApplication::processEvents();

        const auto screenshots = CaptureScreenshots(QGuiApplication::screens());

        if (was_visible) {
            window_to_hide->show();
            window_to_hide->raise();
            window_to_hide->activateWindow();
        }

        QMessageBox::information(
            this,
            "Report error",
            "Screenshots of all monitors have been captured in memory.\n\n"
            "After this dialog is closed, the screenshots will be copied to the clipboard one by one. Please paste and save each one before clicking OK for the next one.\n\n"
            "If you have a GitHub account, please create an issue at:\n"
            "https://github.com/luozhijian/SkipAdClicker/issues\n\n"
            "Otherwise, please send an email to: skipadclicker@gmail.com\n"
            "with all personal information removed.");

        const int copied_count = CopyScreenshotsToClipboard(this, screenshots);
        const QString screenshot_message = copied_count > 0
            ? QString("%1 screenshot%2 copied to the clipboard. Please paste and save it.").arg(copied_count).arg(copied_count == 1 ? "" : "s")
            : QString("Could not copy a screenshot to the clipboard.");

        QMessageBox::information(
            this,
            "Report error",
            screenshot_message + "\n\n"
            "If you have a GitHub account, please create an issue at:\n"
            "https://github.com/luozhijian/SkipAdClicker/issues\n\n"
            "Otherwise, please send an email to: skipadclicker@gmail.com\n"
            "with all personal information removed.");
    });
}

void LogView::EnsureModel()
{
    if (model_ != nullptr) {
        return;
    }
    model_ = new QStandardItemModel();
    model_->setColumnCount(2);
    model_->setHorizontalHeaderLabels({"Time", "Log"});
}

void LogView::AddLog(const QString& message)
{
    EnsureModel();
    auto add = [message]() {
        QList<QStandardItem*> row;
        row << new QStandardItem(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.z"));
        row << new QStandardItem(message);
        model_->appendRow(row);

        while (model_->rowCount() > 1100) {
            model_->removeRows(0, 100);
        }
    };

    if (qApp != nullptr && qApp->thread() != QThread::currentThread()) {
        QMetaObject::invokeMethod(qApp, add, Qt::QueuedConnection);
    } else {
        add();
    }
}

} // namespace automationtest::app
