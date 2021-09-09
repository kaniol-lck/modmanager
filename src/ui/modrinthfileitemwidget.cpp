#include "modrinthfileitemwidget.h"
#include "ui_modrinthfileitemwidget.h"

#include "download/downloadmanager.h"
#include "util/funcutil.h"

ModrinthFileItemWidget::ModrinthFileItemWidget(QWidget *parent, const ModrinthFileInfo &info) :
    QWidget(parent),
    ui(new Ui::ModrinthFileItemWidget),
    fileInfo_(info)
{
    ui->setupUi(this);
    ui->displayNameText->setText(info.displayName());
    ui->downloadProgress->setVisible(false);

    //file name and link
    QString linkText = info.fileName();
    linkText = "<a href=%1>" + linkText + "</a>";
    ui->fileNameText->setText(linkText.arg(info.url().toString()));

    //game version
    QString gameversionText;
    for(const auto &ver : info.gameVersions())
        gameversionText.append(ver).append(" ");
    ui->gameVersionText->setText(gameversionText);

    //loader type
    QString loaderTypeText;
    for(const auto &loader : info.loaderTypes())
        loaderTypeText.append(ModLoaderType::toString(loader)).append(" ");
    ui->loaderTypeText->setText(loaderTypeText);

    //size
    ui->downloadSpeedText->setVisible(false);
}

ModrinthFileItemWidget::~ModrinthFileItemWidget()
{
    delete ui;
}

void ModrinthFileItemWidget::on_downloadButton_clicked()
{
    ui->downloadButton->setText(tr("Downloading"));
    ui->downloadButton->setEnabled(false);
    ui->downloadProgress->setVisible(true);

    ui->downloadProgress->setMaximum(fileInfo_.size());

    auto downloader = DownloadManager::addModDownload(std::make_shared<ModrinthFileInfo>(fileInfo_));
    connect(downloader, &Downloader::downloadProgress, this, [=](qint64 bytesReceived, qint64 /*bytesTotal*/){
        ui->downloadProgress->setValue(bytesReceived);
    });
    connect(downloader, &ModDownloader::downloadSpeed, this, [=](qint64 bytesPerSec){
        ui->downloadSpeedText->setText(numberConvert(bytesPerSec, "B/s"));
    });
    connect(downloader, &Downloader::finished, this, [=]{
        ui->downloadProgress->setVisible(false);
        ui->downloadSpeedText->setText(numberConvert(fileInfo_.size(), "B"));
        ui->downloadButton->setText(tr("Downloaded"));
    });
}

