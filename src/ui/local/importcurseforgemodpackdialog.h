#ifndef IMPORTCURSEFORGEMODPACKDIALOG_H
#define IMPORTCURSEFORGEMODPACKDIALOG_H

#include <QDialog>

#include <quazip.h>

#include "curseforge/curseforgefileinfo.h"
#include "curseforge/curseforgemodinfo.h"
#include "network/reply.hpp"


class QStandardItemModel;
namespace Ui {
class ImportCurseforgeModpackDialog;
}

class ImportCurseforgeModpackDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ImportCurseforgeModpackDialog(QWidget *parent, const QString &fileName);
    ~ImportCurseforgeModpackDialog();

    bool fileValid() const;

private slots:
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_toolButton_clicked();
    void on_ImportCurseforgeModpackDialog_accepted();

private:
    Ui::ImportCurseforgeModpackDialog *ui;
    QStandardItemModel *model_;
    QuaZip zip_;
    QString overrides_;
    bool fileValid_ = false;
};

#endif // IMPORTCURSEFORGEMODPACKDIALOG_H
