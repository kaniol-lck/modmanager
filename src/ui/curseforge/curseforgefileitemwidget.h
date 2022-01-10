#ifndef CURSEFORGEFILEITEMWIDGET_H
#define CURSEFORGEFILEITEMWIDGET_H

#include <QWidget>

#include "curseforge/curseforgefileinfo.h"

class CurseforgeMod;
class LocalMod;
class LocalModPath;
class CurseforgeFileListWidget;
namespace Ui {
class CurseforgeFileItemWidget;
}

class CurseforgeFileItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CurseforgeFileItemWidget(CurseforgeFileListWidget *parent, CurseforgeMod *mod, const CurseforgeFileInfo &info, LocalMod* localMod = nullptr);
    ~CurseforgeFileItemWidget();

public slots:
    void onDownloadPathChanged();
    void updateUi();

private slots:
    void updateLocalInfo();
    void on_downloadButton_clicked();
    void on_CurseforgeFileItemWidget_customContextMenuRequested(const QPoint &pos);

private:
    Ui::CurseforgeFileItemWidget *ui;
    CurseforgeFileListWidget *fileList_ = nullptr;
    CurseforgeMod *mod_;
    LocalMod *localMod_ = nullptr;
    CurseforgeFileInfo fileInfo_;
};

#endif // CURSEFORGEFILEITEMWIDGET_H
