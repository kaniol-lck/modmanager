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
    void downloadFile(const CurseforgeFileInfo &fileInfo);
    void updateUi();
private slots:
    void updateIcon();
    void onDownloadStarted();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadSpeed(qint64 bytesPerSec);
    void onDownloadFinished();
    void on_modSummary_customContextMenuRequested(const QPoint &pos);

private:
    Ui::CurseforgeModItemWidget *ui;
    CurseforgeMod *mod_;
    std::optional<CurseforgeFileInfo> defaultFileInfo_;
    LocalModPath *downloadPath_;
    bool transltedSummary_ = false;
};

#endif // CURSEFORGEMODITEMWIDGET_H
