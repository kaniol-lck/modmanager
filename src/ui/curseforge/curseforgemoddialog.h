#ifndef CURSEFORGEMODDIALOG_H
#define CURSEFORGEMODDIALOG_H

#include <QDialog>

class CurseforgeMod;
class LocalMod;
class LocalModPath;
class QListWidgetItem;
class CurseforgeModInfo;
namespace Ui {
class CurseforgeModDialog;
}

class CurseforgeModDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CurseforgeModDialog(QWidget *parent, CurseforgeMod *mod, LocalMod *localMod = nullptr);
    explicit CurseforgeModDialog(QWidget *parent, const CurseforgeModInfo &modInfo);
    ~CurseforgeModDialog();

signals:
    void downloadPathChanged(LocalModPath *path);

public slots:
    void setDownloadPath(LocalModPath *newDownloadPath);

private slots:
    void updateBasicInfo();
    void updateThumbnail();
    void updateDescription();
    void on_websiteButton_clicked();
    void on_galleryListWidget_itemClicked(QListWidgetItem *item);
    void on_modSummary_customContextMenuRequested(const QPoint &pos);

private:
    Ui::CurseforgeModDialog *ui;
    CurseforgeMod *mod_;
    LocalMod *localMod_ = nullptr;
    LocalModPath *downloadPath_ = nullptr;
    bool transltedSummary_ = false;
};

#endif // CURSEFORGEMODDIALOG_H
