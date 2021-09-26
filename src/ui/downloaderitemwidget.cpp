#include "downloaderitemwidget.h"
#include "ui_downloaderitemwidget.h"

#include <QDir>
#include <QDesktopServices>
#include <QMenu>
#include <QClipboard>
#include <QDebug>

#include "download/moddownloader.h"
#include "util/funcutil.h"

DownloaderItemWidget::DownloaderItemWidget(QWidget *parent, ModDownloader *downloader) :
    QWidget(parent),
    ui(new Ui::DownloaderItemWidget),
    modDownlaoder_(downloader)
{
    ui->setupUi(this);
    ui->downloadSpeedText->setVisible(false);
//    ui->downloadProgress->setVisible(false);

    auto fileInfo = modDownlaoder_->fileInfo();
    ui->displayNameText->setText(fileInfo.displayName());
    ui->downloadSizeText->setText(numberConvert(fileInfo.size(), "B"));

//    QString linkText = fileInfo.fileName();
//    linkText = "<a href=%1>" + linkText + "</a>";
//    ui->fileNameText->setText(linkText.arg(fileInfo.url().toString()));

    if(!fileInfo.icon().isNull())
        ui->downloadIcon->setPixmap(fileInfo.icon().scaled(80, 80, Qt::KeepAspectRatio));

    refreshStatus();

    connect(modDownlaoder_, &ModDownloader::sizeUpdated, this, &DownloaderItemWidget::updateSize);
    connect(modDownlaoder_, &ModDownloader::statusChanged, this, &DownloaderItemWidget::refreshStatus);
    connect(modDownlaoder_, &ModDownloader::downloadProgress, this, &DownloaderItemWidget::downloadProgress);
    connect(modDownlaoder_, &ModDownloader::downloadSpeed, this, &DownloaderItemWidget::downloadSpeed);
    connect(modDownlaoder_, &ModDownloader::finished, this, [=]{
        ui->downloadProgress->setValue(ui->downloadProgress->maximum());
    });
}

DownloaderItemWidget::~DownloaderItemWidget()
{
    delete ui;
}

void DownloaderItemWidget::refreshStatus()
{
    switch (modDownlaoder_->status()) {
    case ModDownloader::Idol:
        ui->downloaStatus->setText(tr("Idol"));
        break;
    case ModDownloader::Queue:
        ui->downloaStatus->setText(tr("Queue"));
        break;
    case ModDownloader::Downloading:
        ui->downloaStatus->setText(tr("Downloading"));
        ui->downloadSpeedText->setVisible(true);
//        ui->downloadProgress->setVisible(true);
        break;
    case ModDownloader::Paused:
        ui->downloaStatus->setText(tr("Paused"));
        break;
    case ModDownloader::Finished:
        ui->downloaStatus->setText(tr("Finished"));
        ui->downloadSpeedText->setVisible(false);
//        ui->downloadProgress->setVisible(false);
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
        QApplication::clipboard()->setText(modDownlaoder_->file().fileName());
    });
    connect(menu->addAction(tr("Copy download link")), &QAction::triggered, this, [=]{
        QApplication::clipboard()->setText(modDownlaoder_->url().toString());
    });
    connect(menu->addAction(QIcon::fromTheme("folder"), tr("Open folder")), &QAction::triggered, this, [=]{
        openFileInFolder(modDownlaoder_->filePath());
    });
    if(!menu->isEmpty())
        menu->exec(mapToGlobal(pos));
}

void DownloaderItemWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    if(modDownlaoder_->status() == ModDownloader::Downloading)
        modDownlaoder_->pauseDownload();
    else if(modDownlaoder_->status() == ModDownloader::Paused)
        modDownlaoder_->resumeDownload();
}
