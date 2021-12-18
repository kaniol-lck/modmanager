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
    void settingsUpdated(LocalModPathInfo info, bool autoLoaderType);

public:
    explicit LocalModPathSettingsDialog(QWidget *parent = nullptr);
    LocalModPathSettingsDialog(QWidget *parent, const LocalModPathInfo &info);

    ~LocalModPathSettingsDialog();

private slots:
    void updateVersionList();
    void on_modDirButton_clicked();
    void on_buttonBox_accepted();
    void on_versionSelect_currentIndexChanged(int index);
    void on_nameText_textEdited(const QString &arg1);
    void on_useAutoName_toggled(bool checked);
    void on_loaderSelect_currentIndexChanged(int index);

private:
    Ui::LocalModPathSettingsDialog *ui;
    LocalModPathInfo info_;
    QString customName_;

    void updateDisplayName();
};

#endif // LOCALMODPATHSETTINGSDIALOG_H
