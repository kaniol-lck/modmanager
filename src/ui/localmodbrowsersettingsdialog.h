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

public slots:
    void updateVersions();

private slots:
    void on_modDirButton_clicked();

    void on_buttonBox_accepted();

    void on_useAutoName_stateChanged(int arg1);

    void on_loaderSelect_currentIndexChanged(const QString &arg1);

    void on_versionSelect_currentIndexChanged(const QString &arg1);

    void on_nameText_textEdited(const QString &arg1);

private:
    Ui::LocalModBrowserSettingsDialog *ui;
    ModDirInfo info_;
    QString customName;

    void updateAutoName();
};

#endif // LOCALMODBROWSERSETTINGSDIALOG_H
