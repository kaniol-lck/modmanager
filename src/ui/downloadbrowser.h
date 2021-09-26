#ifndef DOWNLOADBROWSER_H
#define DOWNLOADBROWSER_H

#include <QWidget>

namespace Ui {
class DownloadBrowser;
}

class Downloader;
class DownloadManager;

class DownloadBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadBrowser(QWidget *parent = nullptr);
    ~DownloadBrowser();

public slots:
    void addNewDownloaderItem(Downloader *downloader);
    void downloadSpeed(qint64 bytesPerSec);

private:
    Ui::DownloadBrowser *ui;
    DownloadManager *manager_;
};

#endif // DOWNLOADBROWSER_H
