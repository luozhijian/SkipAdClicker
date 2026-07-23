#ifndef AUTOMATIOTEST_APP_BROWSERAPPLICATIONSDIALOG_HPP
#define AUTOMATIOTEST_APP_BROWSERAPPLICATIONSDIALOG_HPP

#include <QDialog>
#include <QSet>
#include <QString>

class QTableWidget;

namespace automationtest::app {

class BrowserApplicationsDialog : public QDialog {
public:
    explicit BrowserApplicationsDialog(QWidget* parent = nullptr);

private:
    void AddApplication();
    void RemoveSelectedApplications();
    void Save();

    QTableWidget* applications_table_ {nullptr};
    QSet<QString> installed_identifiers_ {};
};

} // namespace automationtest::app

#endif // AUTOMATIOTEST_APP_BROWSERAPPLICATIONSDIALOG_HPP
