#ifndef DOWNLOADBROWSER_H
#define DOWNLOADBROWSER_H

#include <QWidget>

namespace Ui {
class DownloadBrowser;
}

class ModDownloader;
class DownloadManager;

class DownloadBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadBrowser(QWidget *parent = nullptr);
    ~DownloadBrowser();

public slots:
    void addNewDownloaderItem(ModDownloader *downloader);

private:
    Ui::DownloadBrowser *ui;
    DownloadManager *manager;
};

#endif // DOWNLOADBROWSER_H
