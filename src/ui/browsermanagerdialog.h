#ifndef BROWSERMANAGERDIALOG_H
#define BROWSERMANAGERDIALOG_H

#include <QDialog>

#include "moddirinfo.h"

namespace Ui {
class BrowserManagerDialog;
}

class BrowserManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BrowserManagerDialog(QWidget *parent = nullptr);
    ~BrowserManagerDialog();

private slots:
    void on_upButton_clicked();

    void on_downButton_clicked();

    void on_addButton_clicked();

    void on_deleteButton_clicked();

    void on_BrowserManagerDialog_accepted();

    void on_browserList_currentRowChanged(int currentRow);

    void on_browserList_doubleClicked(const QModelIndex &index);

private:
    void refreshButton();

    Ui::BrowserManagerDialog *ui;

    QList<ModDirInfo> modDirList;
};

#endif // BROWSERMANAGERDIALOG_H
