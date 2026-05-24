#ifndef AUTOMATIOTEST_APP_SERVICES_FILESYSTEMWATCHERSERVICE_HPP
#define AUTOMATIOTEST_APP_SERVICES_FILESYSTEMWATCHERSERVICE_HPP

#include <QFileSystemWatcher>
#include <QObject>
#include <QString>

namespace automationtest::app::services {

class FileSystemWatcherService : public QObject {
public:
    static FileSystemWatcherService& Instance();

    void StartWatch(const QString& path_to_watch, const QString& test_book_folder);

private:
    explicit FileSystemWatcherService(QObject* parent = nullptr);
    void StartAutoRun(const QString& file_path);

    QFileSystemWatcher watcher_ {};
    QString test_book_folder_ {};
};

} // namespace automationtest::app::services

#endif // AUTOMATIOTEST_APP_SERVICES_FILESYSTEMWATCHERSERVICE_HPP
