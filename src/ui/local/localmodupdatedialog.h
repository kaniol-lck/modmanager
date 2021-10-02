#ifndef LOCALMODUPDATEDIALOG_H
#define LOCALMODUPDATEDIALOG_H

#include <QDialog>
#include <QStandardItemModel>

#include "local/localmod.h"

namespace Ui {
class LocalModUpdateDialog;
}

class LocalModPath;
class LocalMod;

class LocalModUpdateDialog : public QDialog
{
    Q_OBJECT
    enum { NameColumn, BeforeColumn, AfterColumn, SourceColumn};
public:
    explicit LocalModUpdateDialog(QWidget *parent, LocalModPath *modPath);
    ~LocalModUpdateDialog();

private slots:
    void onUpdateSourceChanged(int row, LocalMod::ModWebsiteType type);

    void on_LocalModUpdateDialog_accepted();

private:
    Ui::LocalModUpdateDialog *ui;
    LocalModPath *modPath_;
    QList<QPair<LocalMod*, LocalMod::ModWebsiteType>> modUpdateList_;
    QStandardItemModel model_;
};

#endif // LOCALMODUPDATEDIALOG_H
