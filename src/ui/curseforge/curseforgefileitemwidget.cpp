#include "curseforgefileitemwidget.h"
#include "curseforgefilelistwidget.h"
#include "ui_curseforgefileitemwidget.h"

#include <QDebug>
#include <QMenu>
#include <QClipboard>

#include "ui/downloadpathselectmenu.h"
#include "curseforgemoddialog.h"
#include "local/localmod.h"
#include "local/localfilelinker.h"
#include "local/localmodpath.h"
#include "curseforge/curseforgemod.h"
#include "curseforge/curseforgeapi.h"
#include "curseforge/curseforgedependencyinfo.h"
#include "util/funcutil.h"
#include "download/downloadmanager.h"

CurseforgeFileItemWidget::CurseforgeFileItemWidget(CurseforgeFileListWidget *parent, CurseforgeMod *mod, const CurseforgeFileInfo &info, LocalMod *localMod) :
    QWidget(parent),
    ui(new Ui::CurseforgeFileItemWidget),
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
        connect(localMod_, &LocalMod::modCacheUpdated, this, &CurseforgeFileItemWidget::updateLocalInfo);

    onDownloadPathChanged();
    connect(fileList_->downloadPathSelectMenu(), &DownloadPathSelectMenu::DownloadPathChanged, this, &CurseforgeFileItemWidget::onDownloadPathChanged);

    if(fileInfo_.releaseType() == CurseforgeFileInfo::Release){
        ui->releaseType->setText(tr("Release"));
        ui->releaseType->setStyleSheet(QString("color: #fff; background-color: #14b866; border-radius:2px; padding:1px 2px;"));
    } else if(fileInfo_.releaseType() == CurseforgeFileInfo::Beta){
        ui->releaseType->setText(tr("Beta"));
        ui->releaseType->setStyleSheet(QString("color: #fff; background-color: #0e9bd8; border-radius:2px; padding:1px 2px;"));
    }  else if(fileInfo_.releaseType() == CurseforgeFileInfo::Alpha){
        ui->releaseType->setText(tr("Alpha"));
        ui->releaseType->setStyleSheet(QString("color: #fff; background-color: #d3cae8; border-radius:2px; padding:1px 2px;"));
    } else{
        ui->releaseType->setText(QString::number(fileInfo_.releaseType()));
    }
    ui->fileNameText->setText(fileInfo_.fileName());
    ui->dateTimeText->setText(tr("Updated"));
    ui->dateTimeText->setDateTime(info.fileDate());

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
    ui->downloadSpeedText->setText(sizeConvert(fileInfo_.size()));

    //dependencies
    if(fileInfo_.dependencies().isEmpty())
        ui->dependenciesButton->setVisible(false);
    else {
        auto menu = new QMenu;
        ui->dependenciesButton->setMenu(menu);
        connect(menu, &QMenu::aboutToShow, this, [=]{
            if(!menu->isEmpty()) return;
            for(auto &dependencyInfo : fileInfo_.dependencies()){
                if(dependencyInfo.modId()){
                    auto action = menu->addAction(tr("%1 Mod: %2").arg(dependencyInfo.typeString()).arg(dependencyInfo.modId()));
                    auto reply = CurseforgeAPI::api()->getInfo(dependencyInfo.modId());
                    reply.setRunBackground(true);
                    reply.setOnFinished(this, [=](const CurseforgeModInfo &modInfo){
                        action->setText(tr("%1 Mod: %2").arg(dependencyInfo.typeString(), modInfo.name()));
                        connect(action, &QAction::triggered, this, [=]{
                            auto dialog = new CurseforgeModDialog(this, modInfo);
                            dialog->show();
                        });
                    });
                }
            }
        });
    }

    updateUi();
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

    ui->downloadProgress->setMaximum(fileInfo_.size());

    QAria2Downloader *downloader;
    DownloadFileInfo info(fileInfo_);
    info.setIcon(mod_->modInfo().icon());
    info.setTitle(mod_->modInfo().name());
    if(localMod_)
        info.setIcon(localMod_->icon());
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

    connect(downloader, &AbstractDownloader::downloadProgress, this, [=](qint64 bytesReceived, qint64 /*bytesTotal*/){
        ui->downloadProgress->setValue(bytesReceived);
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

void CurseforgeFileItemWidget::onDownloadPathChanged()
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

void CurseforgeFileItemWidget::updateUi()
{
    Config config;
    ui->dateTimeText->setVisible(config.getShowModDateTime());
    ui->loaderTypes->setVisible(config.getShowModLoaderType());
    ui->gameVersions->setVisible(config.getShowModGameVersion());
    ui->releaseType->setVisible(config.getShowModReleaseType());
}

void CurseforgeFileItemWidget::updateLocalInfo()
{
    auto name = fileInfo_.displayName();
    if(localMod_)
        if(const auto &fileInfo = localMod_->modFile()->linker()->curseforgeFileInfo();
                fileInfo && fileInfo->id() == fileInfo_.id())
            name.prepend("<font color=\"#56a\">" + tr("[Current]") + "</font> ");
    ui->displayNameText->setText(name);
    //refresh downloaded infos
    onDownloadPathChanged();
}

void CurseforgeFileItemWidget::on_CurseforgeFileItemWidget_customContextMenuRequested(const QPoint &pos)
{
    auto menu = new QMenu(this);
    connect(menu->addAction(QIcon::fromTheme("edit-copy"), tr("Copy download link")), &QAction::triggered, this, [=]{
        QApplication::clipboard()->setText(fileInfo_.url().toString());
    });

    if(localMod_)
        if(const auto &fileInfo = localMod_->modFile()->linker()->curseforgeFileInfo();
                !fileInfo || fileInfo->id() != fileInfo_.id())
            menu->addAction(tr("Set as current"), this, [=]{
                localMod_->modFile()->linker()->setCurseforgeFileInfo(fileInfo_);
            });
    if(!menu->isEmpty())
        menu->exec(mapToGlobal(pos));
}
