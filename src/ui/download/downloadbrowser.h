#ifndef DOWNLOADBROWSER_H
#define DOWNLOADBROWSER_H

#include <QWidget>

#include "download/downloadfileinfo.h"

namespace Ui {
class DownloadBrowser;
}

class QAria2Downloader;
class QAria2;

class DownloadBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit DownloadBrowser(QWidget *parent = nullptr);
    ~DownloadBrowser();

    static DownloadBrowser *browser;

    QAria2Downloader *addDownload(const DownloadFileInfo &info);

public slots:
    void downloadSpeed(qint64 download, qint64 upload = 0);

private slots:
    void on_pushButton_clicked();

private:
    Ui::DownloadBrowser *ui;
    QAria2 *qaria2_;
};

#endif // DOWNLOADBROWSER_H
