#include "downloadbrowser.h"
#include "ui_downloadbrowser.h"

#include "downloaderitemwidget.h"
#include "download/downloadmanager.h"
#include "util/funcutil.h"

DownloadBrowser::DownloadBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadBrowser),
    manager_(DownloadManager::manager())
{
    ui->setupUi(this);

    connect(manager_, &DownloadManager::downloaderAdded, this, &DownloadBrowser::addNewDownloaderItem);
    connect(manager_, &DownloadManager::downloadSpeed, this, &DownloadBrowser::downloadSpeed);
}

DownloadBrowser::~DownloadBrowser()
{
    delete ui;
}

void DownloadBrowser::addNewDownloaderItem(ModDownloader *downloader)
{
    auto *listItem = new QListWidgetItem();
    listItem->setSizeHint(QSize(500, 100));
    auto widget = new DownloaderItemWidget(this, downloader);
    ui->downloaderListWidget->addItem(listItem);
    ui->downloaderListWidget->setItemWidget(listItem, widget);
}

void DownloadBrowser::downloadSpeed(qint64 bytesPerSec)
{
    auto text = tr("Download Speed: ") + numberConvert(bytesPerSec, "B/s");
    ui->downloadSpeedText->setText(text);
}
