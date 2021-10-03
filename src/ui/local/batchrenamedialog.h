#ifndef BATCHRENAMEDIALOG_H
#define BATCHRENAMEDIALOG_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class BatchRenameDialog;
}

class LocalModPath;
class LocalMod;
class RenameHighlighter;

class BatchRenameDialog : public QDialog
{
    Q_OBJECT
    enum { NameColumn, OldFileNameColumn, NewFileNameColumn};
public:
    explicit BatchRenameDialog(QWidget *parent, LocalModPath *modPath);
    ~BatchRenameDialog();

    bool eventFilter(QObject *obj, QEvent *e);

private slots:
    void updateModList();
    void on_BatchRenameDialog_accepted();
    void on_toolButton_toggled(bool checked);
    void on_toolButton_2_clicked();
    void on_toolButton_3_clicked();
    void on_toolButton_4_clicked();
    void on_toolButton_5_clicked();

    void on_renamePattern_textChanged();

private:
    Ui::BatchRenameDialog *ui;
    LocalModPath *modPath_;
    QList<LocalMod*> modList_;
    QStringList fileNameList_;
    QStandardItemModel model_;
    RenameHighlighter *highlighter_;
};

#endif // BATCHRENAMEDIALOG_H
