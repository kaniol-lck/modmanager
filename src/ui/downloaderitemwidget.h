#ifndef DOWNLOADERITEMWIDGET_H
#define DOWNLOADERITEMWIDGET_H

#include <QWidget>

class ModDownloader;

namespace Ui {
class DownloaderItemWidget;
}

class DownloaderItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloaderItemWidget(QWidget *parent, ModDownloader *downloader);
    ~DownloaderItemWidget();

private slots:
    void refreshStatus();

    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    void downloadSpeed(qint64 bytesPerSec);

    void updateSize(qint64 size);

    void on_DownloaderItemWidget_customContextMenuRequested(const QPoint &pos);

private:
    Ui::DownloaderItemWidget *ui;
    ModDownloader *modDownlaoder_;
};

#endif // DOWNLOADERITEMWIDGET_H
