#ifndef BATCHRENAMEDIALOG_H
#define BATCHRENAMEDIALOG_H

#include <QDialog>
#include <QStandardItemModel>

#include "local/modfilerenamer.h"
#include "local/localmod.h"

class LocalModPath;
class LocalMod;
namespace Ui {
class BatchRenameDialog;
}

class BatchRenameDialog : public QDialog
{
    Q_OBJECT
    enum { NameColumn, OldFileNameColumn, NewFileNameColumn};
public:
    explicit BatchRenameDialog(QWidget *parent = nullptr, QList<LocalMod *> mods = {});
    explicit BatchRenameDialog(QWidget *parent, LocalModPath *modPath);
    ~BatchRenameDialog();
public slots:
    void setMods(QList<LocalMod *> mods);
private slots:
    void on_BatchRenameDialog_accepted();
    void on_toolButton_toggled(bool checked);
    void on_toolButton_2_clicked();
    void on_toolButton_3_clicked();
    void on_toolButton_4_clicked();
    void on_toolButton_5_clicked();
    void on_renamePattern_textChanged();
private:
    Ui::BatchRenameDialog *ui;
    ModFileRenamer renamer_;
//    LocalModPath *modPath_;
    QList<LocalMod*> modList_;
    QStringList fileNameList_;
    QStandardItemModel model_;
};

#endif // BATCHRENAMEDIALOG_H
