#ifndef CURSEFORGEMODINFODIALOG_H
#define CURSEFORGEMODINFODIALOG_H

#include <QDialog>

class CurseforgeMod;
class LocalMod;

namespace Ui {
class CurseforgeModInfoDialog;
}

class CurseforgeModInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CurseforgeModInfoDialog(QWidget *parent, CurseforgeMod *mod, const QString &path, LocalMod *localMod = nullptr);
    ~CurseforgeModInfoDialog();

signals:
    void downloadPathChanged(QString path);

public slots:
    void setDownloadPath(const QString &newDownloadPath);

private:
    Ui::CurseforgeModInfoDialog *ui;
    CurseforgeMod *mod_;
    LocalMod *localMod_ = nullptr;
    QString downloadPath_;
};

#endif // CURSEFORGEMODINFODIALOG_H
