#ifndef AUTOMATIOTEST_APP_LOGVIEW_HPP
#define AUTOMATIOTEST_APP_LOGVIEW_HPP

#include <QDateTime>
#include <QRegularExpression>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QWidget>

namespace automationtest::app {

class LogFilterModel : public QSortFilterProxyModel {
public:
    explicit LogFilterModel(QObject* parent = nullptr);
    void SetPattern(const QString& pattern);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

private:
    QString pattern_ {};
    QRegularExpression regex_ {};
};

class LogView : public QWidget {
public:
    explicit LogView(QWidget* parent = nullptr);

    static void AddLog(const QString& message);
    static void AddLogFromStdString(const std::string& message);
    static void AddLogFromCharPointer(const char* message);

private:
    static void EnsureModel();

    QLineEdit* filter_edit_ {nullptr};
    QTableView* table_ {nullptr};
    LogFilterModel* filter_model_ {nullptr};
    static QStandardItemModel* model_;
};

} // namespace automationtest::app

#endif // AUTOMATIOTEST_APP_LOGVIEW_HPP
