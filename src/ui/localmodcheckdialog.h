#ifndef LOCALMODCHECKDIALOG_H
#define LOCALMODCHECKDIALOG_H

#include <QDialog>

class QTreeWidgetItem;
class LocalModPath;

namespace Ui {
class LocalModCheckDialog;
}

class LocalModCheckDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LocalModCheckDialog(QWidget *parent, LocalModPath *modPath);
    ~LocalModCheckDialog();

private:
    Ui::LocalModCheckDialog *ui;
    LocalModPath *modPath_;
    QTreeWidgetItem *dependsItem_;
    QTreeWidgetItem *conflictsItem_;
    QTreeWidgetItem *breaksItem_;

};

#endif // LOCALMODCHECKDIALOG_H
