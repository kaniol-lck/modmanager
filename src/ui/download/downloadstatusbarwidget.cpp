#include "downloadstatusbarwidget.h"
#include "ui_downloadstatusbarwidget.h"

#include "util/funcutil.h"

DownloadStatusBarWidget::DownloadStatusBarWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadStatusBarWidget)
{
    ui->setupUi(this);
}

DownloadStatusBarWidget::~DownloadStatusBarWidget()
{
    delete ui;
}

void DownloadStatusBarWidget::setDownloadSpeed(qint64 download, qint64 upload)
{
    ui->downloadSpeed->setText(tr("Download: %1").arg(speedConvert(download)));
    ui->uploadSpeed->setText(tr("Upload: %1").arg(speedConvert(upload)));
}
