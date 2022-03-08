#ifndef DOWNLOADERINFOWIDGET_H
#define DOWNLOADERINFOWIDGET_H

#include <QWidget>

class AbstractDownloader;
class SpeedRecorder;
namespace Ui {
class DownloaderInfoWidget;
}

class DownloaderInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloaderInfoWidget(QWidget *parent = nullptr, SpeedRecorder *speedRecorder = nullptr);
    ~DownloaderInfoWidget();

    void setDownloader(AbstractDownloader *downloader);

signals:
    void downloaderChanged();

private slots:
    void onInfoChanged();
    void onStatusChanged(int status);
    void onHandleUpdated();
    void onGlobalDataUpdated();

private:
    Ui::DownloaderInfoWidget *ui;
    SpeedRecorder *globalSpeedRecorder_;
    AbstractDownloader *downloader_;
};

#endif // DOWNLOADERINFOWIDGET_H
