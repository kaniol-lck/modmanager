#include "curseforgemodbrowser.h"
#include "ui_curseforgemodbrowser.h"

#include <QScrollBar>
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QDebug>

#include "util/tutil.hpp"
#include "curseforge/curseforgemod.h"
#include "curseforge/curseforgeapi.h"
#include "curseforgemoditemwidget.h"
#include "curseforgemodinfodialog.h"
#include "gameversion.h"
#include "modloadertype.h"

CurseforgeModBrowser::CurseforgeModBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurseforgeModBrowser)
{
    ui->setupUi(this);

    for(const auto &type : ModLoaderType::curseforge)
        ui->loaderSelect->addItem(ModLoaderType::toString(type));

    connect(ui->modListWidget->verticalScrollBar(), &QAbstractSlider::valueChanged,  this , &CurseforgeModBrowser::onSliderChanged);

    updateVersionList();
    connect(VersionManager::manager(), &VersionManager::curseforgeVersionListUpdated, this, &CurseforgeModBrowser::updateVersionList);

    getModList(currentName_);
    isUiSet_ = true;
}

CurseforgeModBrowser::~CurseforgeModBrowser()
{
    delete ui;
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

void CurseforgeModBrowser::getModList(QString name, int index)
{
    if(!index) currentIndex_ = 0;
    ui->searchButton->setText(tr("Searching..."));
    ui->searchButton->setEnabled(false);
    setCursor(Qt::BusyCursor);

    GameVersion gameVersion = ui->versionSelect->currentIndex()? GameVersion(ui->versionSelect->currentText()) : GameVersion::Any;
    auto sort = ui->sortSelect->currentIndex();

    CurseforgeAPI::searchMods(gameVersion, index, name, sort, [=](const QList<CurseforgeModInfo> &infoList){
        ui->searchButton->setText(tr("&Search"));
        ui->searchButton->setEnabled(true);
        setCursor(Qt::ArrowCursor);

        //new search
        if(currentIndex_ == 0){
            for(auto mod : qAsConst(modList_))
                mod->deleteLater();
            modList_.clear();
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
            auto mod = new CurseforgeMod(this, info);
            modList_.append(mod);

            auto *listItem = new QListWidgetItem();
            listItem->setSizeHint(QSize(500, 100));
            auto version = ui->versionSelect->currentIndex()? GameVersion(ui->versionSelect->currentText()): GameVersion::Any;
            auto loaderType = ModLoaderType::curseforge.at(ui->loaderSelect->currentIndex());
            auto fileInfo = mod->modInfo().latestFileInfo(version, loaderType);
            auto modItemWidget = new CurseforgeModItemWidget(ui->modListWidget, mod, fileInfo);
            ui->modListWidget->addItem(listItem);
            ui->modListWidget->setItemWidget(listItem, modItemWidget);
            auto isShown = loaderType == ModLoaderType::Any || info.loaderTypes().contains(loaderType);
            listItem->setHidden(!isShown);
            if(isShown){
                shownCount++;
                mod->acquireIcon();
            }
        }
        if(shownCount != infoList.count() && shownCount == 0){
            currentIndex_ += 20;
            getModList(currentName_, currentIndex_);
        }

    });
}

void CurseforgeModBrowser::on_modListWidget_doubleClicked(const QModelIndex &index)
{
    auto mod = modList_.at(index.row());
    auto dialog = new CurseforgeModInfoDialog(this, mod);
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
        auto mod = modList_.at(i);
        auto selectedLoaderType = ModLoaderType::curseforge.at(ui->loaderSelect->currentIndex());
        auto isShown = selectedLoaderType == ModLoaderType::Any || mod->modInfo().loaderTypes().contains(selectedLoaderType);
        //hidden -> shown, while not have downloaded thumbnail yet
        if(isHidden && isShown && mod->modInfo().iconBytes().isEmpty())
            mod->acquireIcon();
    }
}

