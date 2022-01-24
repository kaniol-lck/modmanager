#ifndef DOWNLOADBROWSER_H
#define DOWNLOADBROWSER_H

#include "ui/browser.h"
#include "download/downloadfileinfo.h"

namespace Ui {
class DownloadBrowser;
}

class DownloadManager;
class QAria2Downloader;
class QAria2;

class DownloadBrowser : public Browser
{
    Q_OBJECT

public:
    explicit DownloadBrowser(QWidget *parent = nullptr);
    ~DownloadBrowser();

    QIcon icon() const override;
    QString name() const override;
public slots:
    void downloadSpeed(qint64 download, qint64 upload = 0);

private slots:
    void onCurrentRowChanged();
    void on_actionAdd_triggered();
    void updateListViewIndexWidget();
    void on_actionPause_triggered();
    void on_actionStart_triggered();

    void on_actionCopy_Download_Link_triggered();

    void on_actionShow_in_Folder_triggered();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::DownloadBrowser *ui;
    DownloadManager *manager_;
    QMetaObject::Connection conn_;
};

#endif // DOWNLOADBROWSER_H
