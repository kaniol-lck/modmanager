#include "downloaderinfowidget.h"
#include "ui_downloaderinfowidget.h"

#include "download/abstractdownloader.h"
#include "util/funcutil.h"

DownloaderInfoWidget::DownloaderInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloaderInfoWidget)
{
    ui->setupUi(this);
}

DownloaderInfoWidget::~DownloaderInfoWidget()
{
    delete ui;
}

void DownloaderInfoWidget::setDownloader(AbstractDownloader *downloader)
{
    downloader_ = downloader;
    emit downloaderChanged();
    if(!downloader_) return;
    ui->name->setText(downloader->info().displayName());
    ui->speedWidget->setDownloader(downloader);
}
