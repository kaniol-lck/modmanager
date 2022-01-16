#include "qaria2downloaderitemwidget.h"
#include "ui_qaria2downloaderitemwidget.h"

#include <QDebug>
#include <QClipboard>
#include <QMenu>

#include "download/qaria2downloader.h"
#include "util/funcutil.h"

QAria2DownloaderItemWidget::QAria2DownloaderItemWidget(QWidget *parent, QAria2Downloader *downloader) :
    QWidget(parent),
    ui(new Ui::QAria2DownloaderItemWidget),
    downloader_(downloader)
{
    ui->setupUi(this);
    ui->displayNameText->setProperty("class", "Title");

    onInfoChanged();
    connect(downloader_, &AbstractDownloader::infoChanged, this, &QAria2DownloaderItemWidget::onInfoChanged);

    refreshStatus(downloader->status());
    downloadSpeed(0, 0);

    connect(downloader_, &QAria2Downloader::statusChanged, this, &QAria2DownloaderItemWidget::refreshStatus);
    connect(downloader_, &AbstractDownloader::downloadProgress, this, &QAria2DownloaderItemWidget::downloadProgress);
    connect(downloader_, &AbstractDownloader::downloadSpeed, this, &QAria2DownloaderItemWidget::downloadSpeed);
    connect(downloader_, &AbstractDownloader::finished, this, [=]{
        ui->downloadProgress->setVisible(false);
    });
}

QAria2DownloaderItemWidget::~QAria2DownloaderItemWidget()
{
    delete ui;
}

void QAria2DownloaderItemWidget::onInfoChanged()
{
    ui->url->setText(downloader_->info().url().toString());
    ui->filename->setText(downloader_->info().fileName());

    if(!downloader_->info().icon().isNull())
        ui->downloadIcon->setPixmap(downloader_->info().icon().scaled(80, 80, Qt::KeepAspectRatio));
    else
        ui->downloadIcon->setPixmap(QPixmap(":/image/modmanager.png").scaled(80, 80, Qt::KeepAspectRatio));

    if(!downloader_->info().title().isEmpty())
        ui->displayNameText->setText(downloader_->info().title() + R"( <span style="color:gray">()" + downloader_->info().displayName() + ")</span>");
    else if(!downloader_->info().title().isEmpty())
        ui->displayNameText->setText(downloader_->info().title());
    else if(!downloader_->info().displayName().isEmpty())
        ui->displayNameText->setText(downloader_->info().displayName());
    else
        ui->displayNameText->setText("Unnamed");
}

void QAria2DownloaderItemWidget::refreshStatus(int status)
{
    QString text;
    switch (status) {
    case aria2::DOWNLOAD_ACTIVE:
        text = tr("Active");
        ui->downloadProgress->setVisible(true);
        break;
    case aria2::DOWNLOAD_WAITING:
        text = tr("Waiting");
        ui->downloadProgress->setVisible(false);
        break;
    case aria2::DOWNLOAD_PAUSED:
        text = tr("Paused");
        ui->downloadProgress->setVisible(false);
        break;
    case aria2::DOWNLOAD_COMPLETE:
        text = tr("Complete");
        ui->downloadProgress->setVisible(false);
        break;
    case aria2::DOWNLOAD_ERROR:
        text = tr("Error");
        ui->downloadProgress->setVisible(false);
        break;
    case aria2::DOWNLOAD_REMOVED:
        text = tr("Removed");
        ui->downloadProgress->setVisible(false);
        break;
    }
    ui->downloaStatus->setText(text);
}

void QAria2DownloaderItemWidget::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    ui->downloadProgress->setMaximum(bytesTotal);
    ui->downloadProgress->setValue(bytesReceived);
    ui->downloadSizeText->setText(sizeConvert(bytesTotal));
}

void QAria2DownloaderItemWidget::downloadSpeed(qint64 download, qint64 upload)
{
    ui->downloadSpeedText->setVisible(download);
    ui->uploadSpeedText->setVisible(upload);
    if(download)
        ui->downloadSpeedText->setText(speedConvert(download));
    if(upload)
        ui->uploadSpeedText->setText(speedConvert(upload));
}

void QAria2DownloaderItemWidget::on_pauseButton_clicked()
{
    if(downloader_->status() == aria2::DOWNLOAD_ACTIVE)
        downloader_->pause();
    else if(downloader_->status() == aria2::DOWNLOAD_PAUSED)
        downloader_->start();
}

void QAria2DownloaderItemWidget::on_QAria2DownloaderItemWidget_customContextMenuRequested(const QPoint &pos)
{
    auto menu = new QMenu(this);
    menu->addAction(ui->actionCopy_Download_Link);
    menu->addAction(ui->actionShow_Downloaded_File_in_Folder);
    menu->exec(mapToGlobal(pos));
}

void QAria2DownloaderItemWidget::on_actionShow_Downloaded_File_in_Folder_triggered()
{
    openFileInFolder(downloader_->info().fileName(), downloader_->info().path());
}

void QAria2DownloaderItemWidget::on_actionCopy_Download_Link_triggered()
{
    QApplication::clipboard()->setText(downloader_->info().url().toString());
}
