#ifndef LOCALMODUPDATEDIALOG_H
#define LOCALMODUPDATEDIALOG_H

#include <QDialog>
#include <QStandardItemModel>

#include "local/localmod.h"

namespace Ui {
class LocalModUpdateDialog;
}

class LocalModUpdateDialog : public QDialog
{
    Q_OBJECT

    enum { NameColumn, BeforeColumn, AfterColumn};

public:
    explicit LocalModUpdateDialog(QWidget *parent, const QList<LocalMod*> &updateList);
    ~LocalModUpdateDialog();

private slots:

private:
    Ui::LocalModUpdateDialog *ui;
    const QList<LocalMod*> updateList;
    QStandardItemModel model;
};

#endif // LOCALMODUPDATEDIALOG_H
