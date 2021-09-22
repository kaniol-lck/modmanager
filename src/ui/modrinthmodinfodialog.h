#ifndef MODRINTHMODINFODIALOG_H
#define MODRINTHMODINFODIALOG_H

#include <QDialog>

class ModrinthMod;
class LocalMod;
class LocalModPath;

namespace Ui {
class ModrinthModInfoDialog;
}

class ModrinthModInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModrinthModInfoDialog(QWidget *parent, ModrinthMod *mod, LocalMod *localMod = nullptr);
    ~ModrinthModInfoDialog();

signals:
    void downloadPathChanged(LocalModPath *path);

public slots:
    void setDownloadPath(LocalModPath *newDownloadPath);

private:
    Ui::ModrinthModInfoDialog *ui;
    ModrinthMod *mod_;
    LocalMod *localMod_ = nullptr;
    LocalModPath *downloadPath_ = nullptr;
};

#endif // MODRINTHMODINFODIALOG_H
