#ifndef CURSEFORGEMODDIALOG_H
#define CURSEFORGEMODDIALOG_H

#include <QDialog>

class CurseforgeMod;
class LocalMod;
class LocalModPath;
class QListWidgetItem;
namespace Ui {
class CurseforgeModDialog;
}

class CurseforgeModDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CurseforgeModDialog(QWidget *parent, CurseforgeMod *mod, LocalMod *localMod = nullptr);
    ~CurseforgeModDialog();

signals:
    void downloadPathChanged(LocalModPath *path);

public slots:
    void setDownloadPath(LocalModPath *newDownloadPath);

private slots:
    void on_websiteButton_clicked();
    void on_galleryListWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::CurseforgeModDialog *ui;
    CurseforgeMod *mod_;
    LocalMod *localMod_ = nullptr;
    LocalModPath *downloadPath_ = nullptr;
};

#endif // CURSEFORGEMODDIALOG_H
