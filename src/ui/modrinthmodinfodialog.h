#ifndef MODRINTHMODINFODIALOG_H
#define MODRINTHMODINFODIALOG_H

#include <QDialog>

namespace Ui {
class ModrinthModInfoDialog;
}

class ModrinthMod;

class ModrinthModInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModrinthModInfoDialog(QWidget *parent, ModrinthMod *mod);
    ~ModrinthModInfoDialog();

private:
    Ui::ModrinthModInfoDialog *ui;
    ModrinthMod *modrinthMod;
};

#endif // MODRINTHMODINFODIALOG_H
