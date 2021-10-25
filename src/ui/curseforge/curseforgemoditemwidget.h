#ifndef CURSEFORGEMODITEMWIDGET_H
#define CURSEFORGEMODITEMWIDGET_H

#include <QWidget>

#include "curseforge/curseforgefileinfo.h"

class CurseforgeMod;
class LocalModPath;

namespace Ui {
class CurseforgeModItemWidget;
}

class CurseforgeModItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CurseforgeModItemWidget(QWidget *parent, CurseforgeMod *mod, const std::optional<CurseforgeFileInfo> &defaultDownload);
    ~CurseforgeModItemWidget();

    CurseforgeMod *mod() const;

public slots:
    void setDownloadPath(LocalModPath *newDownloadPath);
    void updateUi();
private slots:
    void updateIcon();
    void downloadFile(const CurseforgeFileInfo &fileInfo);
private:
    Ui::CurseforgeModItemWidget *ui;
    CurseforgeMod *mod_;
    std::optional<CurseforgeFileInfo> defaultFileInfo_;
    LocalModPath *downloadPath_;
};

#endif // CURSEFORGEMODITEMWIDGET_H
