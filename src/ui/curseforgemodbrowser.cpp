#include "curseforgemodbrowser.h"
#include "ui_curseforgemodbrowser.h"

#include <QScrollBar>
#include <QDir>
#include <QDesktopServices>
#include <QDebug>

#include "local/localmodpathmanager.h"
#include "local/localmodpath.h"
#include "curseforge/curseforgemod.h"
#include "curseforge/curseforgeapi.h"
#include "curseforgemoditemwidget.h"
#include "curseforgemodinfodialog.h"
#include "gameversion.h"
#include "modloadertype.h"
#include "config.h"
#include "util/funcutil.h"

CurseforgeModBrowser::CurseforgeModBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurseforgeModBrowser),
    api_(new CurseforgeAPI(this))
{
    ui->setupUi(this);

    for(const auto &type : ModLoaderType::curseforge)
        ui->loaderSelect->addItem(ModLoaderType::icon(type), ModLoaderType::toString(type));

    connect(ui->modListWidget->verticalScrollBar(), &QAbstractSlider::valueChanged,  this , &CurseforgeModBrowser::onSliderChanged);

    updateVersionList();
    connect(VersionManager::manager(), &VersionManager::curseforgeVersionListUpdated, this, &CurseforgeModBrowser::updateVersionList);

    updateLocalPathList();
    connect(LocalModPathManager::manager(), &LocalModPathManager::pathListUpdated, this, &CurseforgeModBrowser::updateLocalPathList);

    getModList(currentName_);
    isUiSet_ = true;
}

CurseforgeModBrowser::~CurseforgeModBrowser()
{
    delete ui;
}

void CurseforgeModBrowser::searchModByPathInfo(const LocalModPathInfo &info)
{
    isUiSet_ = false;
    ui->versionSelect->setCurrentText(info.gameVersion());
    ui->loaderSelect->setCurrentIndex(ModLoaderType::curseforge.indexOf(info.loaderType()));
    isUiSet_ = true;
    ui->downloadPathSelect->setCurrentText(info.displayName());
    getModList(currentName_);
}

void CurseforgeModBrowser::updateVersionList()
{
    isUiSet_ = false;
    ui->versionSelect->clear();
    ui->versionSelect->addItem(tr("Any"));
    for(const auto &version : GameVersion::curseforgeVersionList())
        ui->versionSelect->addItem(version);
    isUiSet_ = true;
}

void CurseforgeModBrowser::updateLocalPathList()
{
    //remember selected path
    LocalModPath *selectedPath = nullptr;
    auto index = ui->downloadPathSelect->currentIndex();
    if(index >= 0 && index < LocalModPathManager::pathList().size())
        selectedPath = LocalModPathManager::pathList().at(ui->downloadPathSelect->currentIndex());

    ui->downloadPathSelect->clear();
    downloadPathList_.clear();
    ui->downloadPathSelect->addItem(tr("Custom"));
    downloadPathList_ << Config().getDownloadPath();
    if(downloadPath_.isEmpty()) downloadPath_ = downloadPathList_.first();
    for(const auto &path : LocalModPathManager::pathList()){
        ui->downloadPathSelect->addItem(path->info().displayName());
        downloadPathList_ << path->info().path();
    }

    //reset selected path
    if(selectedPath != nullptr){
        auto index = LocalModPathManager::pathList().indexOf(selectedPath);
        if(index >= 0)
            ui->downloadPathSelect->setCurrentIndex(index);
    }
}

void CurseforgeModBrowser::on_searchButton_clicked()
{
    currentName_ = ui->searchText->text();
    getModList(currentName_);
}

void CurseforgeModBrowser::onSliderChanged(int i)
{
    if(hasMore_ && i == ui->modListWidget->verticalScrollBar()->maximum()){
        currentIndex_ += 20;
        getModList(currentName_, currentIndex_);
    }
}

void CurseforgeModBrowser::getModList(QString name, int index, int needMore)
{
    if(!index) currentIndex_ = 0;
    ui->searchButton->setText(tr("Searching..."));
    ui->searchButton->setEnabled(false);
    setCursor(Qt::BusyCursor);

    GameVersion gameVersion = ui->versionSelect->currentIndex()? GameVersion(ui->versionSelect->currentText()) : GameVersion::Any;
    auto sort = ui->sortSelect->currentIndex();

    api_->searchMods(gameVersion, index, name, sort, [=](const QList<CurseforgeModInfo> &infoList){
        ui->searchButton->setText(tr("&Search"));
        ui->searchButton->setEnabled(true);
        setCursor(Qt::ArrowCursor);

        //new search
        if(currentIndex_ == 0){
            idList_.clear();
            for(int i = 0; i < ui->modListWidget->count(); i++)
                ui->modListWidget->itemWidget(ui->modListWidget->item(i))->deleteLater();
            ui->modListWidget->clear();
            hasMore_ = true;
        }

        if(infoList.isEmpty()){
            hasMore_ = false;
            return ;
        }

        //show them
        int shownCount = 0;
        for(const auto &info : qAsConst(infoList)){
            //do not load duplicate mod
            if(idList_.contains(info.id()))
                continue;
            idList_ << info.id();

            auto mod = new CurseforgeMod(this, info);
            auto *listItem = new QListWidgetItem();
            listItem->setSizeHint(QSize(500, 100));
            auto version = ui->versionSelect->currentIndex()? GameVersion(ui->versionSelect->currentText()): GameVersion::Any;
            auto loaderType = ModLoaderType::curseforge.at(ui->loaderSelect->currentIndex());
            auto fileInfo = mod->modInfo().latestFileInfo(version, loaderType);
            auto modItemWidget = new CurseforgeModItemWidget(ui->modListWidget, mod, fileInfo, downloadPath_);
            connect(this, &CurseforgeModBrowser::downloadPathChanged, modItemWidget, &CurseforgeModItemWidget::setDownloadPath);
            ui->modListWidget->addItem(listItem);
            ui->modListWidget->setItemWidget(listItem, modItemWidget);
            auto isShown = loaderType == ModLoaderType::Any || info.loaderTypes().contains(loaderType);
            listItem->setHidden(!isShown);
            if(isShown){
                shownCount++;
                mod->acquireIcon();
            }
        }
        if(shownCount != infoList.count() && shownCount < needMore){
            currentIndex_ += 20;
            getModList(currentName_, currentIndex_, needMore - shownCount);
        }
    });
}

void CurseforgeModBrowser::on_modListWidget_doubleClicked(const QModelIndex &index)
{
    auto widget = ui->modListWidget->itemWidget(ui->modListWidget->item(index.row()));
    auto mod = dynamic_cast<CurseforgeModItemWidget*>(widget)->mod();
    auto dialog = new CurseforgeModInfoDialog(this, mod, downloadPath_);
    connect(this, &CurseforgeModBrowser::downloadPathChanged, dialog, &CurseforgeModInfoDialog::setDownloadPath);
    dialog->show();
}


void CurseforgeModBrowser::on_versionSelect_currentIndexChanged(int)
{
    if(isUiSet_) getModList(currentName_);
}


void CurseforgeModBrowser::on_sortSelect_currentIndexChanged(int)
{
    if(isUiSet_) getModList(currentName_);
}


void CurseforgeModBrowser::on_loaderSelect_currentIndexChanged(int)
{
    for(int i = 0; i < ui->modListWidget->count(); i++){
        auto item = ui->modListWidget->item(i);
        auto isHidden = item->isHidden();
        auto widget = ui->modListWidget->itemWidget(item);
        auto mod = dynamic_cast<CurseforgeModItemWidget*>(widget)->mod();
        auto selectedLoaderType = ModLoaderType::curseforge.at(ui->loaderSelect->currentIndex());
        auto isShown = selectedLoaderType == ModLoaderType::Any || mod->modInfo().loaderTypes().contains(selectedLoaderType);
        item->setHidden(!isShown);
        //hidden -> shown, while not have downloaded thumbnail yet
        if(isHidden && isShown && mod->modInfo().iconBytes().isEmpty())
            mod->acquireIcon();
    }
}


void CurseforgeModBrowser::on_downloadPathSelect_currentIndexChanged(int index)
{
    if(!isUiSet_ || index < 0 || index >= downloadPathList_.size()) return;
    downloadPath_ = downloadPathList_.at(index);
    emit downloadPathChanged(downloadPath_);
}


void CurseforgeModBrowser::on_openFolderButton_clicked()
{
    openFileInFolder(downloadPath_);
}

