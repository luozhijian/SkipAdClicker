#ifndef AUTOMATIOTEST_APP_BROWSERAPPLICATIONSSETTINGS_HPP
#define AUTOMATIOTEST_APP_BROWSERAPPLICATIONSSETTINGS_HPP

#include <QList>
#include <QString>
#include <QStringList>

#include <any>
#include <string>
#include <unordered_set>

#include "../Utilities/Status/LoadFunctions.hpp"


namespace automationtest::app {

struct BrowserApplicationEntry
{
    QString identifier;
    bool enabled {true};
};

class BrowserApplicationsSettings {
public:
    [[nodiscard]] static bool MergeWithInstalledDefaults(QString* error_message = nullptr);
    [[nodiscard]] static bool Load(QList<BrowserApplicationEntry>& entries, QString* error_message = nullptr);
    [[nodiscard]] static bool LoadInstalledIdentifiers(QStringList& identifiers, QString* error_message = nullptr);
    [[nodiscard]] static bool Save(const QList<BrowserApplicationEntry>& entries, QString* error_message = nullptr);
    static std::any GetBrowserApplications();
    [[nodiscard]] static QString UserFilePath();

    static void RegisterBindings(utilities::status::LoadFunctions& load_functions);
};

} // namespace automationtest::app

#endif // AUTOMATIOTEST_APP_BROWSERAPPLICATIONSSETTINGS_HPP
