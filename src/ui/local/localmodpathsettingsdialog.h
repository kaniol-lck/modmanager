#ifndef LOCALMODPATHSETTINGSDIALOG_H
#define LOCALMODPATHSETTINGSDIALOG_H

#include <QDialog>

#include "local/localmodpathinfo.h"

namespace Ui {
class LocalModPathSettingsDialog;
}

class LocalModPathSettingsDialog : public QDialog
{
    Q_OBJECT

signals:
    void settingsUpdated(LocalModPathInfo info);

public:
    explicit LocalModPathSettingsDialog(QWidget *parent = nullptr);
    LocalModPathSettingsDialog(QWidget *parent, const LocalModPathInfo &info);

    ~LocalModPathSettingsDialog();

private slots:
    void updateVersionList();

    void on_modDirButton_clicked();

    void on_buttonBox_accepted();

    void on_loaderSelect_currentIndexChanged(const QString &arg1);

    void on_versionSelect_currentIndexChanged(const QString &arg1);

    void on_nameText_textEdited(const QString &arg1);

    void on_useAutoName_toggled(bool checked);

private:
    Ui::LocalModPathSettingsDialog *ui;
    LocalModPathInfo info_;
    QString customName;

    void updateAutoName();
};

#endif // LOCALMODPATHSETTINGSDIALOG_H