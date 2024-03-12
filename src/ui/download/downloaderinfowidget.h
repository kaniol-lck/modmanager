#ifndef DOWNLOADERINFOWIDGET_H
#define DOWNLOADERINFOWIDGET_H

#include <QWidget>

class AbstractDownloader;
namespace Ui {
class DownloaderInfoWidget;
}

class DownloaderInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloaderInfoWidget(QWidget *parent = nullptr);
    ~DownloaderInfoWidget();

    void setDownloader(AbstractDownloader *downloader);

signals:
    void downloaderChanged();

private slots:
    void onInfoChanged();
    void onStatusChanged(int status);
    void onHandleUpdated();

private:
    Ui::DownloaderInfoWidget *ui;
    AbstractDownloader *downloader_;
};

#endif // DOWNLOADERINFOWIDGET_H
