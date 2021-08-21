#ifndef CURSEFORGEMODINFODIALOG_H
#define CURSEFORGEMODINFODIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

class QNetworkAccessManager;
class CurseforgeMod;
class QNetworkReply;

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
    CurseforgeMod *curseforgeMod;
    QNetworkAccessManager *accessManager;
    QStandardItemModel model;
    QSortFilterProxyModel proxyModel;
};

#endif // CURSEFORGEMODINFODIALOG_H
