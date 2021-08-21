#ifndef LOCALMODINFODIALOG_H
#define LOCALMODINFODIALOG_H

#include <QDialog>

#include "curseforge/curseforgemod.h"

class LocalMod;

namespace Ui {
class LocalModInfoDialog;
}

class LocalModInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LocalModInfoDialog(QWidget *parent, LocalMod *mod);
    ~LocalModInfoDialog();

private slots:
    void on_curseforgeButton_clicked();

private:
    Ui::LocalModInfoDialog *ui;
    LocalMod *localMod;
};

#endif // LOCALMODINFODIALOG_H
