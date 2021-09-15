#ifndef LOCALMODBROWSERSETTINGSDIALOG_H
#define LOCALMODBROWSERSETTINGSDIALOG_H

#include <QDialog>

#include "local/localmodpathinfo.h"

namespace Ui {
class LocalModBrowserSettingsDialog;
}

class LocalModBrowserSettingsDialog : public QDialog
{
    Q_OBJECT

signals:
    void settingsUpdated(LocalModPathInfo info);

public:
    explicit LocalModBrowserSettingsDialog(QWidget *parent = nullptr);
    LocalModBrowserSettingsDialog(QWidget *parent, const LocalModPathInfo &info);

    ~LocalModBrowserSettingsDialog();

private slots:
    void updateVersionList();

    void on_modDirButton_clicked();

    void on_buttonBox_accepted();

    void on_loaderSelect_currentIndexChanged(const QString &arg1);

    void on_versionSelect_currentIndexChanged(const QString &arg1);

    void on_nameText_textEdited(const QString &arg1);

    void on_useAutoName_toggled(bool checked);

private:
    Ui::LocalModBrowserSettingsDialog *ui;
    LocalModPathInfo info_;
    QString customName;

    void updateAutoName();
};

#endif // LOCALMODBROWSERSETTINGSDIALOG_H
