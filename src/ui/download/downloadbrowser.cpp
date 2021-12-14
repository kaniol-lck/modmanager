#include "downloadbrowser.h"
#include "ui_downloadbrowser.h"

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
    Browser(parent),
    ui(new Ui::DownloadBrowser),
    manager_(DownloadManager::manager())
{
    ui->setupUi(this);
    ui->downloaderListWidget->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->downloaderListWidget->setProperty("class", "ModList");

    connect(manager_->qaria2(), &QAria2::started, this, [=]{});
    connect(manager_->qaria2(), &QAria2::finished, this, [=]{
        ui->statusbar->showMessage("Idoling");
    });
    connect(manager_, &DownloadManager::downloaderAdded, this, &DownloadBrowser::addNewDownloaderItem);
    connect(manager_->qaria2(), &QAria2::downloadSpeed, this, &DownloadBrowser::downloadSpeed);
}

DownloadBrowser::~DownloadBrowser()
{
    delete ui;
}

QIcon DownloadBrowser::icon() const
{
    return QIcon::fromTheme("download");
}

QString DownloadBrowser::name() const
{
    return tr("Downloader");
}

void DownloadBrowser::addNewDownloaderItem(const DownloadFileInfo &info, QAria2Downloader *downloader)
{
    auto *listItem = new QListWidgetItem();
    auto widget = new QAria2DownloaderItemWidget(this, downloader, info);
    listItem->setSizeHint(QSize(0, widget->height()));
    ui->downloaderListWidget->addItem(listItem);
    ui->downloaderListWidget->setItemWidget(listItem, widget);
}

void DownloadBrowser::downloadSpeed(qint64 download, qint64 upload)
{
    auto text = tr("Download Speed:") + speedConvert(download) +
            " " + tr("Upload Speed:") + speedConvert(upload);
    ui->statusbar->showMessage(text);
}
