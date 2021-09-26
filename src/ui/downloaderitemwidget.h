#ifndef DOWNLOADERITEMWIDGET_H
#define DOWNLOADERITEMWIDGET_H

#include <QWidget>

#include "download/downloader.h"

class ModDownloader;

namespace Ui {
class DownloaderItemWidget;
}

class DownloaderItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloaderItemWidget(QWidget *parent, Downloader *downloader);
    ~DownloaderItemWidget();

private slots:
    void refreshStatus(Downloader::DownloadStatus status);

    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    void downloadSpeed(qint64 bytesPerSec);

    void updateSize(qint64 size);

    void on_DownloaderItemWidget_customContextMenuRequested(const QPoint &pos);
protected:
    void mouseDoubleClickEvent(QMouseEvent*);

private:
    Ui::DownloaderItemWidget *ui;
    Downloader *downloader_;
};

#endif // DOWNLOADERITEMWIDGET_H
