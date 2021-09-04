#include "downloadbrowser.h"
#include "ui_downloadbrowser.h"

#include "downloaderitemwidget.h"
#include "download/downloadmanager.h"

DownloadBrowser::DownloadBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadBrowser),
    manager(DownloadManager::manager())
{
    ui->setupUi(this);

    connect(manager, &DownloadManager::downloaderAdded, this, &DownloadBrowser::addNewDownloaderItem);
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
