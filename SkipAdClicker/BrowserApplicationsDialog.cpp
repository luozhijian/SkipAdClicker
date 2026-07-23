#include "BrowserApplicationsDialog.hpp"

#include "BrowserApplicationsSettings.hpp"

#include <QAbstractItemView>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSet>
#include <QTableWidget>
#include <QVBoxLayout>

#include <algorithm>

namespace automationtest::app {

namespace {

QString IdentifierDescription()
{
#if defined(Q_OS_WIN)
    return QStringLiteral("Enter one executable filename per row, for example chrome.exe.");
#elif defined(Q_OS_LINUX)
    return QStringLiteral("Enter one application ID per row, for example org.mozilla.firefox.");
#elif defined(Q_OS_MACOS)
    return QStringLiteral("Enter one bundle ID per row, for example com.apple.Safari.");
#else
    return QStringLiteral("Enter one application identifier per row.");
#endif
}

} // namespace

BrowserApplicationsDialog::BrowserApplicationsDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Edit Browser Applications"));
    resize(620, 520);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(IdentifierDescription(), this));

    applications_table_ = new QTableWidget(this);
    applications_table_->setColumnCount(2);
    applications_table_->setHorizontalHeaderLabels({QStringLiteral("Enabled"), QStringLiteral("Application identifier")});
    applications_table_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    applications_table_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    applications_table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    applications_table_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    layout->addWidget(applications_table_);

    auto* edit_buttons = new QHBoxLayout();
    auto* add_button = new QPushButton(QStringLiteral("Add"), this);
    auto* remove_button = new QPushButton(QStringLiteral("Remove"), this);
    remove_button->setToolTip(QStringLiteral("Custom entries are removed. Installed defaults are disabled."));
    edit_buttons->addWidget(add_button);
    edit_buttons->addWidget(remove_button);
    edit_buttons->addStretch();
    layout->addLayout(edit_buttons);

    auto* dialog_buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    layout->addWidget(dialog_buttons);

    connect(add_button, &QPushButton::clicked, this, [this]() { AddApplication(); });
    connect(remove_button, &QPushButton::clicked, this, [this]() { RemoveSelectedApplications(); });
    connect(dialog_buttons->button(QDialogButtonBox::Save), &QPushButton::clicked, this, [this]() { Save(); });
    connect(dialog_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QList<BrowserApplicationEntry> entries;
    QString error_message;
    if (!BrowserApplicationsSettings::Load(entries, &error_message)) {
        QMessageBox::warning(this, QStringLiteral("SkipAdClicker"), error_message);
        return;
    }
    QStringList installed_identifiers;
    if (!BrowserApplicationsSettings::LoadInstalledIdentifiers(installed_identifiers, &error_message)) {
        QMessageBox::warning(this, QStringLiteral("SkipAdClicker"), error_message);
        return;
    }
    for (const auto& identifier : installed_identifiers) {
        installed_identifiers_.insert(identifier.toCaseFolded());
    }

    for (const auto& entry : entries) {
        const int row = applications_table_->rowCount();
        applications_table_->insertRow(row);

        auto* enabled_item = new QTableWidgetItem();
        enabled_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
        enabled_item->setCheckState(entry.enabled ? Qt::Checked : Qt::Unchecked);
        applications_table_->setItem(row, 0, enabled_item);
        applications_table_->setItem(row, 1, new QTableWidgetItem(entry.identifier));
    }
}

void BrowserApplicationsDialog::AddApplication()
{
    const int row = applications_table_->rowCount();
    applications_table_->insertRow(row);

    auto* enabled_item = new QTableWidgetItem();
    enabled_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
    enabled_item->setCheckState(Qt::Checked);
    applications_table_->setItem(row, 0, enabled_item);
    applications_table_->setItem(row, 1, new QTableWidgetItem());
    applications_table_->setCurrentCell(row, 1);
    applications_table_->editItem(applications_table_->item(row, 1));
}

void BrowserApplicationsDialog::RemoveSelectedApplications()
{
    QList<int> rows;
    for (const auto& selected_range : applications_table_->selectedRanges()) {
        for (int row = selected_range.topRow(); row <= selected_range.bottomRow(); ++row) {
            if (!rows.contains(row)) {
                rows.append(row);
            }
        }
    }

    std::sort(rows.begin(), rows.end(), std::greater<int>());
    for (const int row : rows) {
        const auto* identifier_item = applications_table_->item(row, 1);
        const auto identifier = identifier_item != nullptr ? identifier_item->text().trimmed().toCaseFolded() : QString {};
        if (installed_identifiers_.contains(identifier)) {
            if (auto* enabled_item = applications_table_->item(row, 0); enabled_item != nullptr) {
                enabled_item->setCheckState(Qt::Unchecked);
            }
        } else {
            applications_table_->removeRow(row);
        }
    }
}

void BrowserApplicationsDialog::Save()
{
    QList<BrowserApplicationEntry> entries;
    QSet<QString> identifiers;

    for (int row = 0; row < applications_table_->rowCount(); ++row) {
        const auto* identifier_item = applications_table_->item(row, 1);
        const auto identifier = identifier_item != nullptr ? identifier_item->text().trimmed() : QString {};
        if (identifier.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("SkipAdClicker"), QString("Application identifier on row %1 is empty.").arg(row + 1));
            applications_table_->setCurrentCell(row, 1);
            return;
        }
        if (identifier.contains('/') || identifier.contains('\\') || identifier.contains('=')) {
            QMessageBox::warning(
                this,
                QStringLiteral("SkipAdClicker"),
                QString("Application identifier '%1' contains an unsupported character.").arg(identifier));
            applications_table_->setCurrentCell(row, 1);
            return;
        }

        const auto normalized_identifier = identifier.toCaseFolded();
        if (identifiers.contains(normalized_identifier)) {
            QMessageBox::warning(this, QStringLiteral("SkipAdClicker"), QString("Application identifier '%1' is duplicated.").arg(identifier));
            applications_table_->setCurrentCell(row, 1);
            return;
        }
        identifiers.insert(normalized_identifier);

        const auto* enabled_item = applications_table_->item(row, 0);
        entries.append(BrowserApplicationEntry {
            .identifier = identifier,
            .enabled = enabled_item != nullptr && enabled_item->checkState() == Qt::Checked,
        });
    }

    QString error_message;
    if (!BrowserApplicationsSettings::Save(entries, &error_message)) {
        QMessageBox::warning(this, QStringLiteral("SkipAdClicker"), error_message);
        return;
    }
    accept();
}

} // namespace automationtest::app
