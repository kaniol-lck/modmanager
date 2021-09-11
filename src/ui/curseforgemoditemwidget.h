#ifndef CURSEFORGEMODITEMWIDGET_H
#define CURSEFORGEMODITEMWIDGET_H

#include <QWidget>

#include "curseforge/curseforgefileinfo.h"

class CurseforgeMod;

namespace Ui {
class CurseforgeModItemWidget;
}

class CurseforgeModItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CurseforgeModItemWidget(QWidget *parent, CurseforgeMod *mod, const std::optional<CurseforgeFileInfo> &defaultDownload, const QString &path);
    ~CurseforgeModItemWidget();

    CurseforgeMod *mod() const;

public slots:
    void setDownloadPath(const QString &newDownloadPath);

private slots:
    void updateIcon();

    void downloadFile(const CurseforgeFileInfo &fileInfo);

private:
    Ui::CurseforgeModItemWidget *ui;
    CurseforgeMod *mod_;
    std::optional<CurseforgeFileInfo> defaultFileInfo_;
    QString downloadPath_;
};

#endif // CURSEFORGEMODITEMWIDGET_H
