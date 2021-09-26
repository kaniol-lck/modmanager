#include "downloaderitemwidget.h"
#include "ui_downloaderitemwidget.h"

#include <QDir>
#include <QDesktopServices>
#include <QMenu>
#include <QClipboard>
#include <QDebug>

#include "download/moddownloader.h"
#include "util/funcutil.h"

DownloaderItemWidget::DownloaderItemWidget(QWidget *parent, Downloader *downloader) :
    QWidget(parent),
    ui(new Ui::DownloaderItemWidget),
    downloader_(downloader)
{
    ui->setupUi(this);
    ui->downloadSpeedText->setVisible(false);
//    ui->downloadProgress->setVisible(false);

    //TODO
//    auto fileInfo = qobject_cast<ModDownloader*>(downloader_)->fileInfo();
    ui->displayNameText->setText(downloader_->file().fileName());
    ui->downloadSizeText->setText(numberConvert(downloader_->size(), "B"));

//    QString linkText = fileInfo.fileName();
//    linkText = "<a href=%1>" + linkText + "</a>";
//    ui->fileNameText->setText(linkText.arg(fileInfo.url().toString()));

    //TODO
//    if(!fileInfo.icon().isNull())
//        ui->downloadIcon->setPixmap(fileInfo.icon().scaled(80, 80, Qt::KeepAspectRatio));

    refreshStatus(downloader->status());

    connect(downloader_, &Downloader::sizeUpdated, this, &DownloaderItemWidget::updateSize);
    connect(downloader_, &Downloader::statusChanged, this, &DownloaderItemWidget::refreshStatus);
    connect(downloader_, &Downloader::downloadProgress, this, &DownloaderItemWidget::downloadProgress);
    connect(downloader_, &Downloader::downloadSpeed, this, &DownloaderItemWidget::downloadSpeed);
    connect(downloader_, &Downloader::finished, this, [=]{
        ui->downloadProgress->setValue(ui->downloadProgress->maximum());
    });
}

DownloaderItemWidget::~DownloaderItemWidget()
{
    delete ui;
}

void DownloaderItemWidget::refreshStatus(Downloader::DownloadStatus status)
{
    switch (status) {
    case Downloader::Idol:
        ui->downloaStatus->setText(tr("Idol"));
        break;
    case Downloader::Perparing:
        ui->downloaStatus->setText(tr("Perparing"));
        break;
    case Downloader::Queue:
        ui->downloaStatus->setText(tr("Queue"));
        break;
    case Downloader::Downloading:
        ui->downloaStatus->setText(tr("Downloading"));
        ui->downloadSpeedText->setVisible(true);
//        ui->downloadProgress->setVisible(true);
        break;
    case Downloader::Paused:
        ui->downloaStatus->setText(tr("Paused"));
        break;
    case Downloader::Finished:
        ui->downloaStatus->setText(tr("Finished"));
        ui->downloadSpeedText->setVisible(false);
//        ui->downloadProgress->setVisible(false);
        break;
    case Downloader::Error:
        ui->downloaStatus->setText(tr("Error"));
        ui->downloadSpeedText->setVisible(false);
        break;
    }
}

void DownloaderItemWidget::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    ui->downloadProgress->setMaximum(bytesTotal);
    ui->downloadProgress->setValue(bytesReceived);
}

void DownloaderItemWidget::downloadSpeed(qint64 bytesPerSec)
{
    ui->downloadSpeedText->setText(numberConvert(bytesPerSec, "B/s"));
}

void DownloaderItemWidget::updateSize(qint64 size)
{
    ui->downloadSizeText->setText(numberConvert(size, "B"));
}

void DownloaderItemWidget::on_DownloaderItemWidget_customContextMenuRequested(const QPoint &pos)
{
    auto menu = new QMenu(this);
    connect(menu->addAction(tr("Copy file name")), &QAction::triggered, this, [=]{
        QApplication::clipboard()->setText(downloader_->file().fileName());
    });
    connect(menu->addAction(tr("Copy download link")), &QAction::triggered, this, [=]{
        QApplication::clipboard()->setText(downloader_->url().toString());
    });
    connect(menu->addAction(QIcon::fromTheme("folder"), tr("Open folder")), &QAction::triggered, this, [=]{
        openFileInFolder(downloader_->file().fileName());
    });
    if(!menu->isEmpty())
        menu->exec(mapToGlobal(pos));
}

void DownloaderItemWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    if(downloader_->status() == ModDownloader::Downloading)
        downloader_->pauseDownload();
    else if(downloader_->status() == ModDownloader::Paused)
        downloader_->resumeDownload();
}
