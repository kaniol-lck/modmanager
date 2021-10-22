#include "downloadbrowser.h"
#include "ui_downloadbrowser.h"

#include <QtConcurrent>
#include <QInputDialog>
#include <QDebug>

#include "ui/download/qaria2downloaderitemwidget.h"
#include "download/qaria2.h"
#include "download/qaria2downloader.h"
#include "util/funcutil.h"

DownloadBrowser *DownloadBrowser::browser = nullptr;

DownloadBrowser::DownloadBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadBrowser),
    qaria2_(QAria2::qaria2())
{
    browser = this;
    ui->setupUi(this);
    ui->downloadSpeedText->setText("?");

    connect(qaria2_, &QAria2::started, this, [=]{});
    connect(qaria2_, &QAria2::finished, this, [=]{
        ui->downloadSpeedText->setText("Idoling");
    });
//    connect(qaria2_, &QAria2::downloaderAdded, this, &DownloadBrowser::addNewDownloaderItem);
    connect(qaria2_, &QAria2::downloadSpeed, this, &DownloadBrowser::downloadSpeed);
}

DownloadBrowser::~DownloadBrowser()
{
    delete ui;
}

QAria2Downloader *DownloadBrowser::addDownload(const DownloadFileInfo &info)
{
    auto downloader = new QAria2Downloader(info.url(), info.path());
    auto *listItem = new QListWidgetItem();
    listItem->setSizeHint(QSize(0, 108));
    auto widget = new QAria2DownloaderItemWidget(this, downloader);
    ui->downloaderListWidget->addItem(listItem);
    ui->downloaderListWidget->setItemWidget(listItem, widget);
    //handle redirect
    downloader->handleRedirect();
    connect(downloader, &AbstractDownloader::redirected, qaria2_, [=]{
        qaria2_->download(downloader);
    });
    return downloader;
}

void DownloadBrowser::downloadSpeed(qint64 download, qint64 upload)
{
    auto text = tr("Download Speed:") + numberConvert(download, "B/s") +
            " " + tr("Upload Speed:") + numberConvert(upload, "B/s");
    ui->downloadSpeedText->setText(text);
}

void DownloadBrowser::on_pushButton_clicked()
{
    auto str = QInputDialog::getText(this, "", "Url");
    auto url = AbstractDownloader::handleRedirect(str);
    qDebug() << str << url;
    qaria2_->download(url);
}

