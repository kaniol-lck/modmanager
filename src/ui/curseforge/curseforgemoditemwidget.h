#ifndef CURSEFORGEMODITEMWIDGET_H
#define CURSEFORGEMODITEMWIDGET_H

#include <QWidget>

#include "curseforge/curseforgefileinfo.h"

class CurseforgeMod;
class CurseforgeModBrowser;
class LocalModPath;

namespace Ui {
class CurseforgeModItemWidget;
}

class CurseforgeModItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CurseforgeModItemWidget(CurseforgeModBrowser *parent, CurseforgeMod *mod, const std::optional<CurseforgeFileInfo> &defaultDownload);
    ~CurseforgeModItemWidget();

    CurseforgeMod *mod() const;

public slots:
    void onDownloadPathChanged();
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
    CurseforgeModBrowser *browser_ = nullptr;
    CurseforgeMod *mod_;
    std::optional<CurseforgeFileInfo> defaultFileInfo_;
    bool transltedSummary_ = false;
};

#endif // CURSEFORGEMODITEMWIDGET_H
