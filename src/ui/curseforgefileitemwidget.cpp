#include "curseforgefileitemwidget.h"
#include "ui_curseforgefileitemwidget.h"

#include <QDebug>

#include "util/funcutil.h"
#include "download/downloadmanager.h"

CurseforgeFileItemWidget::CurseforgeFileItemWidget(QWidget *parent, const CurseforgeFileInfo &info) :
    QWidget(parent),
    ui(new Ui::CurseforgeFileItemWidget),
    curseforgeFileInfo(info)
{
    ui->setupUi(this);
    ui->displayNameText->setText(curseforgeFileInfo.getDisplayName());
    ui->downloadProgress->setVisible(false);
    //file name and link
    QString linkText = curseforgeFileInfo.getFileName();
    linkText = "<a href=%1>" + linkText + "</a>";
    ui->fileNameText->setText(linkText.arg(curseforgeFileInfo.getUrl().toString()));

    //game version
    QString gameversionText;
    for(const auto &ver : curseforgeFileInfo.getGameVersions())
        gameversionText.append(ver).append(" ");
    ui->gameVersionText->setText(gameversionText);

    //loader type
    QString loaderTypeText;
    for(const auto &loader : curseforgeFileInfo.getModLoaders())
        loaderTypeText.append(ModLoaderType::toString(loader)).append(" ");
    ui->loaderTypeText->setText(loaderTypeText);

    //size
    ui->downloadSpeedText->setText(numberConvert(curseforgeFileInfo.getSize(), "B"));
}

CurseforgeFileItemWidget::~CurseforgeFileItemWidget()
{
    delete ui;
}

void CurseforgeFileItemWidget::on_downloadButton_clicked()
{
    ui->downloadButton->setText(tr("Downloading"));
    ui->downloadButton->setEnabled(false);
    ui->downloadProgress->setVisible(true);

    ui->downloadProgress->setMaximum(curseforgeFileInfo.getSize());

    auto downloader = DownloadManager::addModDownload(std::make_shared<CurseforgeFileInfo>(curseforgeFileInfo));
    connect(downloader, &Downloader::downloadProgress, this, [=](qint64 bytesReceived, qint64 /*bytesTotal*/){
        ui->downloadProgress->setValue(bytesReceived);
    });
    connect(downloader, &ModDownloader::downloadSpeed, this, [=](qint64 bytesPerSec){
        ui->downloadSpeedText->setText(numberConvert(bytesPerSec, "B/s"));
    });
    connect(downloader, &Downloader::finished, this, [=]{
        ui->downloadProgress->setVisible(false);
        ui->downloadSpeedText->setText(numberConvert(curseforgeFileInfo.getSize(), "B"));
        ui->downloadButton->setText(tr("Downloaded"));
    });
}
