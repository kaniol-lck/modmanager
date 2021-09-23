#include "optifinemodbrowser.h"
#include "ui_optifinemodbrowser.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>

#include "optifinemoditemwidget.h"
#include "optifine/optifineapi.h"
#include "optifine/optifinemod.h"
#include "local/localmodpath.h"
#include "local/localmodpathmanager.h"
#include "util/funcutil.h"
#include "config.h"

OptifineModBrowser::OptifineModBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OptifineModBrowser),
    api_(new OptifineAPI(this))
{
    ui->setupUi(this);

    getModList();

    updateLocalPathList();
    connect(LocalModPathManager::manager(), &LocalModPathManager::pathListUpdated, this, &OptifineModBrowser::updateLocalPathList);

    isUiSet_ = true;
}

OptifineModBrowser::~OptifineModBrowser()
{
    delete ui;
}

void OptifineModBrowser::updateLocalPathList()
{
    //remember selected path
    LocalModPath *selectedPath = nullptr;
    auto index = ui->downloadPathSelect->currentIndex();
    if(index >= 0 && index < LocalModPathManager::pathList().size())
        selectedPath = LocalModPathManager::pathList().at(ui->downloadPathSelect->currentIndex());

    ui->downloadPathSelect->clear();
    ui->downloadPathSelect->addItem(tr("Custom"));
    for(const auto &path : LocalModPathManager::pathList())
        ui->downloadPathSelect->addItem(path->info().displayName());

    //reset selected path
    if(selectedPath != nullptr){
        auto index = LocalModPathManager::pathList().indexOf(selectedPath);
        if(index >= 0)
            ui->downloadPathSelect->setCurrentIndex(index);
    }
}

void OptifineModBrowser::getModList()
{
    api_->getModList([=](const auto &list){
        for(auto modInfo : list){
            auto mod = new OptifineMod(this, modInfo);
            mod->acquireDownloadUrl();
            auto *listItem = new QListWidgetItem();
            listItem->setSizeHint(QSize(500, 100));
            ui->modListWidget->addItem(listItem);
            auto itemWidget = new OptifineModItemWidget(this, mod);
            ui->modListWidget->setItemWidget(listItem, itemWidget);
        }
    });
}

void OptifineModBrowser::on_openFolderButton_clicked()
{
    QString path;
    if(downloadPath_)
        path = downloadPath_->info().path();
    else
        path = Config().getDownloadPath();
    openFileInFolder(path);
}

void OptifineModBrowser::on_downloadPathSelect_currentIndexChanged(int index)
{
    if(!isUiSet_ || index < 0 || index >= ui->downloadPathSelect->count()) return;
    if(index == 0)
        downloadPath_ = nullptr;
    else
        downloadPath_ =  LocalModPathManager::pathList().at(index - 1);
    emit downloadPathChanged(downloadPath_);
}

void OptifineModBrowser::on_checkBox_stateChanged(int arg1)
{
    //TODO
}

void OptifineModBrowser::on_versionSelect_currentIndexChanged(int index)
{
    //TODO
}

void OptifineModBrowser::on_searchText_textEdited(const QString &arg1)
{
    for(int i = 0; i < ui->modListWidget->count(); i++){
        auto widget = ui->modListWidget->itemWidget(ui->modListWidget->item(i));
        auto mod = dynamic_cast<const OptifineModItemWidget*>(widget)->mod();
        auto str = arg1.toLower();
        if(mod->modInfo().name().toLower().contains(str))
            ui->modListWidget->item(i)->setHidden(false);
        else
            ui->modListWidget->item(i)->setHidden(true);
    }
}

