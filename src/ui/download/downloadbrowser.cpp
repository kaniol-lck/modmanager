#include "downloadbrowser.h"
#include "ui_downloadbrowser.h"

#include <QtConcurrent>
#include <QInputDialog>
#include <QDebug>
#include <QToolBar>

#include "ui/download/qaria2downloaderitemwidget.h"
#include "download/downloadmanager.h"
#include "download/qaria2.h"
#include "download/qaria2downloader.h"
#include "util/funcutil.h"
#include "util/smoothscrollbar.h"

DownloadBrowser::DownloadBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadBrowser),
    manager_(DownloadManager::manager())
{
    ui->setupUi(this);
    ui->downloaderListWidget->setVerticalScrollBar(new SmoothScrollBar(this));

    connect(manager_->qaria2(), &QAria2::started, this, [=]{});
    connect(manager_->qaria2(), &QAria2::finished, this, [=]{
        ui->downloadSpeedText->setText("Idoling");
    });
    connect(manager_, &DownloadManager::downloaderAdded, this, &DownloadBrowser::addNewDownloaderItem);
    connect(manager_->qaria2(), &QAria2::downloadSpeed, this, &DownloadBrowser::downloadSpeed);
}

DownloadBrowser::~DownloadBrowser()
{
    delete ui;
}

void DownloadBrowser::addNewDownloaderItem(const DownloadFileInfo &info, QAria2Downloader *downloader)
{
    auto *listItem = new QListWidgetItem();
    listItem->setSizeHint(QSize(0, 108));
    auto widget = new QAria2DownloaderItemWidget(this, downloader, info);
    ui->downloaderListWidget->addItem(listItem);
    ui->downloaderListWidget->setItemWidget(listItem, widget);
}

void DownloadBrowser::downloadSpeed(qint64 download, qint64 upload)
{
    auto text = tr("Download Speed:") + numberConvert(download, "B/s") +
            " " + tr("Upload Speed:") + numberConvert(upload, "B/s");
    ui->downloadSpeedText->setText(text);
}
