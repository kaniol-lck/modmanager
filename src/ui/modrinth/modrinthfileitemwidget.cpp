#include "modrinthfileitemwidget.h"
#include "modrinthfilelistwidget.h"
#include "ui_modrinthfileitemwidget.h"

#include <QMenu>
#include <QClipboard>

#include "ui/downloadpathselectmenu.h"
#include "local/localmod.h"
#include "local/localmodpath.h"
#include "local/localfilelinker.h"
#include "modrinth/modrinthmod.h"
#include "download/downloadmanager.h"
#include "util/funcutil.h"

ModrinthFileItemWidget::ModrinthFileItemWidget(ModrinthFileListWidget *parent, ModrinthMod *mod, const ModrinthFileInfo &info, LocalMod *localMod) :
    QWidget(parent),
    ui(new Ui::ModrinthFileItemWidget),
    fileList_(parent),
    mod_(mod),
    localMod_(localMod),
    fileInfo_(info)
{
    ui->setupUi(this);
    ui->displayNameText->setProperty("class", "Title");
    ui->downloadProgress->setVisible(false);

    updateLocalInfo();
    if(localMod_)
        connect(localMod_, &LocalMod::modCacheUpdated, this, &ModrinthFileItemWidget::updateLocalInfo);

    onDownloadPathChanged();
    connect(fileList_->downloadPathSelectMenu(), &DownloadPathSelectMenu::DownloadPathChanged, this, &ModrinthFileItemWidget::onDownloadPathChanged);

    if(fileInfo_.releaseType() == ModrinthFileInfo::Release){
        ui->releaseType->setText(tr("Release"));
        ui->releaseType->setStyleSheet(QString("color: #fff; background-color: #14b866; border-radius:2px; padding:1px 2px;"));
    } else if(fileInfo_.releaseType() == ModrinthFileInfo::Beta){
        ui->releaseType->setText(tr("Beta"));
        ui->releaseType->setStyleSheet(QString("color: #fff; background-color: #0e9bd8; border-radius:2px; padding:1px 2px;"));
    }  else if(fileInfo_.releaseType() == ModrinthFileInfo::Alpha){
        ui->releaseType->setText(tr("Alpha"));
        ui->releaseType->setStyleSheet(QString("color: #fff; background-color: #d3cae8; border-radius:2px; padding:1px 2px;"));
    } else{
        ui->releaseType->setText(fileInfo_.releaseType());
    }
    ui->fileNameText->setText(fileInfo_.fileName());
    ui->fileDateTime->setText(tr("Updated"));
    ui->fileDateTime->setDateTime(info.fileDate());

    //game version
    for(auto &&version : fileInfo_.gameVersions()){
        auto label = new QLabel(this);
        label->setText(version);
        label->setToolTip(version);
        if(version.isDev())
            label->setStyleSheet(QString("color: #fff; background-color: #735e82; border-radius:8px; padding:1px 2px;"));
        else
            label->setStyleSheet(QString("color: #fff; background-color: #827965; border-radius:8px; padding:1px 2px;"));
        auto font = label->font();
        font.setPointSize(9);
        label->setFont(font);
        ui->versionsLayout->addWidget(label);
    }

    //loader type
    for(auto &&loaderType : fileInfo_.loaderTypes()){
        auto label = new QLabel(this);
        if(loaderType == ModLoaderType::Fabric)
            label->setText(QString(R"(<img src=":/image/fabric.png" height="22" width="22"/>)"));
        else if(loaderType == ModLoaderType::Forge)
            label->setText(QString(R"(<img src=":/image/forge.svg" height="22" width="22"/>)"));
        else
            label->setText(ModLoaderType::toString(loaderType));
        label->setToolTip(ModLoaderType::toString(loaderType));
        ui->loadersLayout->addWidget(label);
    }

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

    QAria2Downloader *downloader;
    DownloadFileInfo info(fileInfo_);
    info.setIcon(mod_->modInfo().icon());
    info.setTitle(mod_->modInfo().name());
    if(localMod_)
        info.setIconBytes(localMod_->commonInfo()->iconBytes());
    else
        info.setIcon(mod_->modInfo().icon());
    if(auto downloadPath = fileList_->downloadPathSelectMenu()->downloadPath())
        downloader = downloadPath->downloadNewMod(info);
    else if(localMod_)
        downloader = localMod_->downloadOldMod(info);
    else{
        info.setPath(Config().getDownloadPath());
        downloader = DownloadManager::manager()->download(info);
    }
    connect(downloader, &AbstractDownloader::downloadProgress, this, [=](qint64 bytesReceived, qint64 bytesTotal){
        ui->downloadProgress->setValue(bytesReceived);
        ui->downloadProgress->setMaximum(bytesTotal);
    });
    connect(downloader, &AbstractDownloader::downloadSpeed, this, [=](qint64 bytesPerSec){
        ui->downloadSpeedText->setText(speedConvert(bytesPerSec));
    });
    connect(downloader, &AbstractDownloader::finished, this, [=]{
        ui->downloadProgress->setVisible(false);
        ui->downloadSpeedText->setText(sizeConvert(fileInfo_.size()));
        ui->downloadButton->setText(tr("Downloaded"));
    });
}

void ModrinthFileItemWidget::onDownloadPathChanged()
{
    bool bl = false;
    if(fileList_){
        if(auto downloadPath = fileList_->downloadPathSelectMenu()->downloadPath())
            bl = hasFile(downloadPath, fileInfo_);
    }
    if(!bl)
        bl = hasFile(Config().getDownloadPath(), fileInfo_.fileName());

    if(bl){
        ui->downloadButton->setEnabled(false);
        ui->downloadButton->setText(tr("Downloaded"));
    } else{
        ui->downloadButton->setEnabled(true);
        ui->downloadButton->setText(tr("Download"));
    }
}

void ModrinthFileItemWidget::updateLocalInfo()
{
    auto name = fileInfo_.displayName();
    if(localMod_)
        if(const auto &fileInfo = localMod_->modFile()->linker()->modrinthFileInfo();
                fileInfo && fileInfo->id() == fileInfo_.id())
        name.prepend("<font color=\"#56a\">" + tr("[Current]") + "</font> ");
    ui->displayNameText->setText(name);
    //refresh downloaded infos
    onDownloadPathChanged();
}

void ModrinthFileItemWidget::on_ModrinthFileItemWidget_customContextMenuRequested(const QPoint &pos)
{
    auto menu = new QMenu(this);
    connect(menu->addAction(QIcon::fromTheme("edit-copy"), tr("Copy download link")), &QAction::triggered, this, [=]{
        QApplication::clipboard()->setText(fileInfo_.url().toString());
    });
    if(localMod_)
        if(const auto &fileInfo = localMod_->modFile()->linker()->modrinthFileInfo();
                !fileInfo || fileInfo->id() != fileInfo_.id())
            menu->addAction(tr("Set as current"), this, [=]{
                localMod_->modFile()->linker()->setModrinthFileInfo(fileInfo_);
            });
    if(!menu->isEmpty())
        menu->exec(mapToGlobal(pos));
}
