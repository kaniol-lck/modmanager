#ifndef LOCALMODBROWSERSETTINGSDIALOG_H
#define LOCALMODBROWSERSETTINGSDIALOG_H

#include <QDialog>
#include <QDir>

#include "moddirinfo.h"

namespace Ui {
class LocalModBrowserSettingsDialog;
}

class LocalModBrowserSettingsDialog : public QDialog
{
    Q_OBJECT

signals:
    void settingsUpdated(ModDirInfo info);

public:
    explicit LocalModBrowserSettingsDialog(QWidget *parent = nullptr);
    LocalModBrowserSettingsDialog(QWidget *parent, const ModDirInfo &info);

    ~LocalModBrowserSettingsDialog();

private slots:
    void on_modDirButton_clicked();

    void on_buttonBox_accepted();

private:
    Ui::LocalModBrowserSettingsDialog *ui;
};

#endif // LOCALMODBROWSERSETTINGSDIALOG_H
