#ifndef CURSEFORGEFILEITEMWIDGET_H
#define CURSEFORGEFILEITEMWIDGET_H

#include <QWidget>

#include "curseforge/curseforgefileinfo.h"

class CurseforgeMod;
class LocalMod;
class LocalModPath;

namespace Ui {
class CurseforgeFileItemWidget;
}

class CurseforgeFileItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CurseforgeFileItemWidget(QWidget *parent, CurseforgeMod *mod, const CurseforgeFileInfo &info, LocalMod* localMod = nullptr);
    ~CurseforgeFileItemWidget();

public slots:
    void setDownloadPath(LocalModPath *newDownloadPath);

private slots:
    void on_downloadButton_clicked();

private:
    Ui::CurseforgeFileItemWidget *ui;
    CurseforgeMod *mod_;
    LocalMod *localMod_ = nullptr;
    CurseforgeFileInfo fileInfo_;

    LocalModPath *downloadPath_ = nullptr;
};

#endif // CURSEFORGEFILEITEMWIDGET_H
