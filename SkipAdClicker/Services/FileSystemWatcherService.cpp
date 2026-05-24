#include "FileSystemWatcherService.hpp"

#include "../LogView.hpp"
#include "../RecentFiles.hpp"
#include "../StartUp.hpp"
#include "../../TestBookLib/Parser/TestBookParser.hpp"
#include "../../Utilities/Services/VariableService.hpp"

#include <QFile>
#include <QTextStream>
#include <QThread>

namespace automationtest::app::services {

FileSystemWatcherService& FileSystemWatcherService::Instance()
{
    static FileSystemWatcherService instance;
    return instance;
}

FileSystemWatcherService::FileSystemWatcherService(QObject* parent)
    : QObject(parent)
{
    connect(&watcher_, &QFileSystemWatcher::fileChanged, this, [this](const QString& path) {
        StartAutoRun(path);
    });
}

void FileSystemWatcherService::StartWatch(const QString& path_to_watch, const QString& test_book_folder)
{
    test_book_folder_ = test_book_folder;
    RecentFiles::Instance().AddRecentFile(test_book_folder);
    watcher_.addPath(path_to_watch);
    LogView::AddLog(QString("StartWatch %1 %2").arg(path_to_watch, test_book_folder));
}

void FileSystemWatcherService::StartAutoRun(const QString& file_path)
{
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream stream(&file);
    int index = 1;
    while (!stream.atEnd()) {
        const auto variable_name = "$variable" + std::to_string(index);
        utilities::services::VariableService::Instance().SetObject(variable_name, stream.readLine().toStdString());
        ++index;
    }
    file.close();
    QFile::remove(file_path);

    try {
        StartUp::OpenOneFolder(test_book_folder_.toStdString());
        LogView::AddLog("Completed.");
    } catch (const std::exception& exception) {
        LogView::AddLog(QString("Failed to run %1: %2").arg(file_path, exception.what()));
    }
}

} // namespace automationtest::app::services
