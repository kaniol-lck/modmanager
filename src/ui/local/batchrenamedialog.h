#ifndef BATCHRENAMEDIALOG_H
#define BATCHRENAMEDIALOG_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class BatchRenameDialog;
}

class LocalModPath;
class LocalMod;

class BatchRenameDialog : public QDialog
{
    Q_OBJECT
    enum { NameColumn, OldFileNameColumn, NewFileNameColumn};
public:
    explicit BatchRenameDialog(QWidget *parent, LocalModPath *modPath);
    ~BatchRenameDialog();

private slots:
    void on_renamePattern_textChanged(const QString &arg1);

private:
    Ui::BatchRenameDialog *ui;
    LocalModPath *modPath_;
    QList<LocalMod*> modList_;
    QStandardItemModel model_;
};

#endif // BATCHRENAMEDIALOG_H
