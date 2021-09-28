#include "replaymodbrowser.h"
#include "ui_replaymodbrowser.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "replaymoditemwidget.h"
#include "replay/replayapi.h"
#include "replay/replaymod.h"
#include "local/localmodpath.h"
#include "local/localmodpathmanager.h"
#include "util/funcutil.h"
#include "config.h"

ReplayModBrowser::ReplayModBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReplayModBrowser),
    api_(new ReplayAPI())
{
    ui->setupUi(this);

    for(const auto &type : ModLoaderType::replay)
        ui->loaderSelect->addItem(ModLoaderType::icon(type), ModLoaderType::toString(type));

    getModList();

    updateLocalPathList();
    connect(LocalModPathManager::manager(), &LocalModPathManager::pathListUpdated, this, &ReplayModBrowser::updateLocalPathList);

    connect(ui->versionSelect, &QComboBox::currentTextChanged, this, &ReplayModBrowser::filterList);
    connect(ui->loaderSelect, &QComboBox::currentTextChanged, this, &ReplayModBrowser::filterList);
    connect(ui->searchText, &QLineEdit::textChanged, this, &ReplayModBrowser::filterList);
    isUiSet_ = true;
}

ReplayModBrowser::~ReplayModBrowser()
{
    delete ui;
}

void ReplayModBrowser::searchModByPathInfo(const LocalModPathInfo &info)
{
    isUiSet_ = false;
    ui->versionSelect->setCurrentText(info.gameVersion());
    ui->loaderSelect->setCurrentIndex(ModLoaderType::curseforge.indexOf(info.loaderType()));
    isUiSet_ = true;
    ui->downloadPathSelect->setCurrentText(info.displayName());
    filterList();
}

void ReplayModBrowser::updateLocalPathList()
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

void ReplayModBrowser::on_openFolderButton_clicked()
{
    QString path;
    if(downloadPath_)
        path = downloadPath_->info().path();
    else
        path = Config().getDownloadPath();
    openFileInFolder(path);
}

void ReplayModBrowser::on_downloadPathSelect_currentIndexChanged(int index)
{
    if(!isUiSet_ || index < 0 || index >= ui->downloadPathSelect->count()) return;
    if(index == 0)
        downloadPath_ = nullptr;
    else
        downloadPath_ =  LocalModPathManager::pathList().at(index - 1);
    emit downloadPathChanged(downloadPath_);
}

void ReplayModBrowser::getModList()
{
    api_->getModList([=](const auto &list){
        ui->modListWidget->clear();
        QStringList gameVersions;
        for(auto modInfo : list){
            if(!gameVersions.contains(modInfo.gameVersion()))
                gameVersions << modInfo.gameVersion();
            auto mod = new ReplayMod(this, modInfo);
            auto *listItem = new QListWidgetItem();
            listItem->setSizeHint(QSize(0, 100));
            ui->modListWidget->addItem(listItem);
            auto itemWidget = new ReplayModItemWidget(this, mod);
            itemWidget->setDownloadPath(downloadPath_);
            connect(this, &ReplayModBrowser::downloadPathChanged, itemWidget, &ReplayModItemWidget::setDownloadPath);
            ui->modListWidget->setItemWidget(listItem, itemWidget);
        }
        ui->versionSelect->clear();
        ui->versionSelect->addItem(tr("Any"));
        ui->versionSelect->addItems(gameVersions);

        auto item = new QListWidgetItem(tr("There is no more mod here..."));
        item->setSizeHint(QSize(0, 100));
        auto font = qApp->font();
        font.setPointSize(20);
        item->setFont(font);
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->modListWidget->addItem(item);
    });
}

void ReplayModBrowser::filterList()
{
    auto gameVersion = ui->versionSelect->currentIndex()? GameVersion(ui->versionSelect->currentText()) : GameVersion::Any;
    auto loaderType = ModLoaderType::curseforge.at(ui->loaderSelect->currentIndex());
    auto searchText = ui->searchText->text().toLower();
    for(int i = 0; i < ui->modListWidget->count(); i++){
        auto item = ui->modListWidget->item(i);
        if(!item->text().isEmpty()) continue;
        auto widget = ui->modListWidget->itemWidget(item);
        auto mod = dynamic_cast<ReplayModItemWidget*>(widget)->mod();
        item->setHidden((loaderType != ModLoaderType::Any && mod->modInfo().loaderType() != loaderType) ||
                        (gameVersion != GameVersion::Any && gameVersion != mod->modInfo().gameVersion()) ||
                        !(mod->modInfo().name().toLower().contains(searchText) ||
                          mod->modInfo().gameVersionString().contains(searchText)));
    }
}
