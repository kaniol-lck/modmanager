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
class DownloadStatusBarWidget;
class DownloaderInfoWidget;
class DownloadBrowser : public Browser
{
    Q_OBJECT
public:
    explicit DownloadBrowser(QWidget *parent = nullptr);
    ~DownloadBrowser();

    QWidget *infoWidget() const override;

    QIcon icon() const override;
    QString name() const override;

private slots:
    void onSelectionChanged();
    void on_actionAdd_triggered();
    void updateListViewIndexWidget();
    void on_actionPause_triggered();
    void on_actionStart_triggered();
    void on_actionCopy_Download_Link_triggered();
    void on_actionShow_in_Folder_triggered();
    void on_downloaderListView_customContextMenuRequested(const QPoint &pos);
    void on_actionForce_Pause_triggered();
    void on_actionStop_triggered();
    void on_actionForce_Stop_triggered();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::DownloadBrowser *ui;
    DownloadManager *manager_;
    QMetaObject::Connection conn_;
    DownloadStatusBarWidget *statusBarWidget_;
    DownloaderInfoWidget *infoWidget_;
};

#endif // DOWNLOADBROWSER_H
