#ifndef LOCALMODINFODIALOG_H
#define LOCALMODINFODIALOG_H

#include <QDialog>

#include "local/localmodinfo.h"

class LocalMod;

namespace Ui {
class LocalModInfoDialog;
}

class LocalModInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LocalModInfoDialog(QWidget *parent, const LocalModInfo &modInfo, LocalMod *mod = nullptr);
    ~LocalModInfoDialog();

public slots:
    void updateInfo();

private slots:
    void on_curseforgeButton_clicked();

    void on_modrinthButton_clicked();

    void on_websiteButton_clicked();

    void on_sourceButton_clicked();

    void on_issueButton_clicked();

    void on_editFileNameButton_clicked();

    void on_fileBaseNameText_editingFinished();

    void on_oldModListWidget_doubleClicked(const QModelIndex &index);

private:
    Ui::LocalModInfoDialog *ui;
    LocalModInfo modInfo_;
    LocalMod *mod_;
};

#endif // LOCALMODINFODIALOG_H
