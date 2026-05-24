#include "RecentFiles.hpp"

#include <QSettings>

namespace automationtest::app {

RecentFiles& RecentFiles::Instance()
{
    static RecentFiles instance;
    return instance;
}

RecentFiles::RecentFiles()
{
    recent_files_ = QSettings().value("RecentTestBooks").toStringList();
}

QStringList RecentFiles::RecentFileList() const
{
    return recent_files_;
}

void RecentFiles::AddRecentFile(const QString& recent_file)
{
    auto normalized = recent_file.trimmed();
    while (normalized.endsWith('\\') || normalized.endsWith('/')) {
        normalized.chop(1);
    }
    if (normalized.isEmpty()) {
        return;
    }

    for (const auto& existing : recent_files_) {
        if (QString::compare(existing, normalized, Qt::CaseInsensitive) == 0) {
            return;
        }
    }

    recent_files_.prepend(normalized);
    while (recent_files_.size() > 20) {
        recent_files_.removeLast();
    }
    WriteRecentList();
}

void RecentFiles::WriteRecentList() const
{
    QSettings().setValue("RecentTestBooks", recent_files_);
}

} // namespace automationtest::app
