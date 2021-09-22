#ifndef CURSEFORGEMODINFODIALOG_H
#define CURSEFORGEMODINFODIALOG_H

#include <QDialog>

class CurseforgeMod;
class LocalMod;
class LocalModPath;

namespace Ui {
class CurseforgeModInfoDialog;
}

class CurseforgeModInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CurseforgeModInfoDialog(QWidget *parent, CurseforgeMod *mod, LocalMod *localMod = nullptr);
    ~CurseforgeModInfoDialog();

signals:
    void downloadPathChanged(LocalModPath *path);

public slots:
    void setDownloadPath(LocalModPath *newDownloadPath);

private:
    Ui::CurseforgeModInfoDialog *ui;
    CurseforgeMod *mod_;
    LocalMod *localMod_ = nullptr;
    LocalModPath *downloadPath_ = nullptr;
};

#endif // CURSEFORGEMODINFODIALOG_H
