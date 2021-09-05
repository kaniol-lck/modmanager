#ifndef CURSEFORGEMODINFODIALOG_H
#define CURSEFORGEMODINFODIALOG_H

#include <QDialog>

class CurseforgeMod;

namespace Ui {
class CurseforgeModInfoDialog;
}

class CurseforgeModInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CurseforgeModInfoDialog(QWidget *parent, CurseforgeMod *mod);
    ~CurseforgeModInfoDialog();

private:
    Ui::CurseforgeModInfoDialog *ui;
    CurseforgeMod *mod_;
};

#endif // CURSEFORGEMODINFODIALOG_H
