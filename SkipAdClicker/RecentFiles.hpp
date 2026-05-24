#ifndef AUTOMATIOTEST_APP_RECENTFILES_HPP
#define AUTOMATIOTEST_APP_RECENTFILES_HPP

#include <QStringList>

namespace automationtest::app {

class RecentFiles {
public:
    static RecentFiles& Instance();

    [[nodiscard]] QStringList RecentFileList() const;
    void AddRecentFile(const QString& recent_file);

private:
    RecentFiles();
    void WriteRecentList() const;

    QStringList recent_files_ {};
};

} // namespace automationtest::app

#endif // AUTOMATIOTEST_APP_RECENTFILES_HPP
