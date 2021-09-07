#include "modrinthmodbrowser.h"
#include "ui_modrinthmodbrowser.h"

#include <QScrollBar>

#include "modrinthmoditemwidget.h"
#include "modrinthmodinfodialog.h"
#include "modrinth/modrinthapi.h"
#include "gameversion.h"
#include "modloadertype.h"

ModrinthModBrowser::ModrinthModBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModrinthModBrowser)
{
    ui->setupUi(this);

    for(const auto &type : ModLoaderType::modrinth)
        ui->loaderSelect->addItem(ModLoaderType::toString(type));

    connect(ui->modListWidget->verticalScrollBar(), &QAbstractSlider::valueChanged,  this , &ModrinthModBrowser::onSliderChanged);

    updateVersionList();
    connect(VersionManager::manager(), &VersionManager::modrinthVersionListUpdated, this, &ModrinthModBrowser::updateVersionList);

    getModList(currentName_);
    isUiSet_ = true;
}

ModrinthModBrowser::~ModrinthModBrowser()
{
    delete ui;
}

void ModrinthModBrowser::updateVersionList()
{
    isUiSet_ = false;
    ui->versionSelect->clear();
    ui->versionSelect->addItem(tr("Any"));
    for(const auto &version : GameVersion::modrinthVersionList())
        ui->versionSelect->addItem(version);
    isUiSet_ = true;
}

void ModrinthModBrowser::on_searchButton_clicked()
{
    currentName_ = ui->searchText->text();
    getModList(currentName_);
}

void ModrinthModBrowser::onSliderChanged(int i)
{
    if(hasMore_ && i == ui->modListWidget->verticalScrollBar()->maximum()){
        currentIndex_ += 20;
        getModList(currentName_, currentIndex_);
    }
}

void ModrinthModBrowser::getModList(QString name, int index)
{
    if(!index) currentIndex_ = 0;
    ui->searchButton->setText(tr("Searching..."));
    ui->searchButton->setEnabled(false);
    setCursor(Qt::BusyCursor);

    GameVersion gameVersion = ui->versionSelect->currentIndex()? GameVersion(ui->versionSelect->currentText()) : GameVersion::Any;
    auto sort = ui->sortSelect->currentIndex();
    GameVersion version = ui->versionSelect->currentIndex()? GameVersion(ui->versionSelect->currentText()) : GameVersion::Any;
    auto type = ModLoaderType::modrinth.at(ui->loaderSelect->currentIndex());

    ModrinthAPI::searchMods(name, currentIndex_, version, type, sort, [=](const QList<ModrinthModInfo> &infoList){
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
        for(const auto &info : qAsConst(infoList)){
            auto modrinthMod = new ModrinthMod(this, info);
            modList_.append(modrinthMod);

            auto *listItem = new QListWidgetItem();
            listItem->setSizeHint(QSize(500, 100));
            auto version = ui->versionSelect->currentIndex()? GameVersion(ui->versionSelect->currentText()): GameVersion::Any;
            auto modItemWidget = new ModrinthModItemWidget(ui->modListWidget, modrinthMod);
            ui->modListWidget->addItem(listItem);
            ui->modListWidget->setItemWidget(listItem, modItemWidget);
            modrinthMod->acquireIcon();
        }
    });
}


void ModrinthModBrowser::on_modListWidget_doubleClicked(const QModelIndex &index)
{
    auto mod = modList_.at(index.row());
    auto dialog = new ModrinthModInfoDialog(this, mod);
    dialog->show();
}


void ModrinthModBrowser::on_sortSelect_currentIndexChanged(int)
{
    if(isUiSet_) getModList(currentName_);
}


void ModrinthModBrowser::on_versionSelect_currentIndexChanged(int)
{
    if(isUiSet_) getModList(currentName_);
}


void ModrinthModBrowser::on_loaderSelect_currentIndexChanged(int)
{
    if(isUiSet_) getModList(currentName_);
}

