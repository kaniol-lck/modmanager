#include "replaymodbrowser.h"
#include "ui_replaymodbrowser.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QScrollBar>
#include <QStandardItem>
#include <QStatusBar>

#include "ui/explorestatusbarwidget.h"
#include "replaymoditemwidget.h"
#include "replay/replayapi.h"
#include "replay/replaymod.h"
#include "local/localmodpath.h"
#include "local/localmodpathmanager.h"
#include "util/funcutil.h"
#include "util/smoothscrollbar.h"
#include "config.hpp"

ReplayModBrowser::ReplayModBrowser(QWidget *parent) :
    ExploreBrowser(parent, QIcon(":/image/replay.png"), "ReplayMod", QUrl("https://www.replaymod.com")),
    ui(new Ui::ReplayModBrowser),
    api_(new ReplayAPI())
{
    ui->setupUi(this);
    initUi();

    //setup status bar
    ui->statusbar->addPermanentWidget(statusBarWidget_);

    ui->searchBar->addWidget(ui->searchText);

    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->label);
    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->versionSelect);
    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->label_3);
    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->loaderSelect);
    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->label_4);
    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->downloadPathSelect);

    for(const auto &type : ModLoaderType::replay)
        ui->loaderSelect->addItem(ModLoaderType::icon(type), ModLoaderType::toString(type));

    updateLocalPathList();
    updateStatusText();
    connect(LocalModPathManager::manager(), &LocalModPathManager::pathListUpdated, this, &ReplayModBrowser::updateLocalPathList);

    connect(ui->versionSelect, &QComboBox::currentTextChanged, this, &ReplayModBrowser::filterList);
    connect(ui->loaderSelect, &QComboBox::currentTextChanged, this, &ReplayModBrowser::filterList);
    connect(ui->searchText, &QLineEdit::textChanged, this, &ReplayModBrowser::filterList);

    if(Config().getSearchModsOnStartup()){
        inited_ = true;
        getModList();
    }
}

ReplayModBrowser::~ReplayModBrowser()
{
    delete ui;
}

void ReplayModBrowser::load()
{
    if(!inited_){
        inited_ = true;
        getModList();
    }
}

void ReplayModBrowser::refresh()
{
    getModList();
}

void ReplayModBrowser::searchModByPathInfo(const LocalModPathInfo &info)
{
    ui->versionSelect->setCurrentText(info.gameVersion());
    ui->loaderSelect->setCurrentIndex(ModLoaderType::curseforge.indexOf(info.loaderType()));
    ui->downloadPathSelect->setCurrentText(info.displayName());
    filterList();
}

void ReplayModBrowser::updateUi()
{

}

ExploreBrowser *ReplayModBrowser::another()
{
    return new ReplayModBrowser;
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

void ReplayModBrowser::on_downloadPathSelect_currentIndexChanged(int index)
{
    if(index < 0 || index >= ui->downloadPathSelect->count()) return;
    if(index == 0)
        downloadPath_ = nullptr;
    else
        downloadPath_ =  LocalModPathManager::pathList().at(index - 1);
    emit downloadPathChanged(downloadPath_);
}

void ReplayModBrowser::getModList()
{
    setCursor(Qt::BusyCursor);
    statusBarWidget_->setText(tr("Searching mods..."));
    statusBarWidget_->setProgressVisible(true);

    api_->getModList([=](const auto &list){
        setCursor(Qt::ArrowCursor);
        statusBarWidget_->setText("");
        statusBarWidget_->setProgressVisible(false);
        for(auto row = 0; row < model_->rowCount(); row++){
            auto item = model_->item(row);
            auto mod = item->data().value<ReplayMod*>();
            if(mod && !mod->parent())
                mod->deleteLater();
        }
        model_->clear();
        QStringList gameVersions;
        for(auto modInfo : list){
            if(!gameVersions.contains(modInfo.gameVersion()))
                gameVersions << modInfo.gameVersion();
            auto mod = new ReplayMod(this, modInfo);
            auto item = new QStandardItem;
            item->setData(QVariant::fromValue(mod));
            model_->appendRow(item);
            item->setSizeHint(QSize(0, 100));
        }
        ui->versionSelect->clear();
        ui->versionSelect->addItem(tr("Any"));
        ui->versionSelect->addItems(gameVersions);

        auto item = new QStandardItem(tr("There is no more mod here..."));
        item->setSizeHint(QSize(0, 108));
        auto font = qApp->font();
        font.setPointSize(20);
        item->setFont(font);
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        model_->appendRow(item);

        updateStatusText();
    });
}

QWidget *ReplayModBrowser::getIndexWidget(QStandardItem *item)
{
    auto mod = item->data().value<ReplayMod*>();
    if(mod){
        auto widget = new ReplayModItemWidget(this, mod);
        widget->setDownloadPath(downloadPath_);
        connect(this, &ReplayModBrowser::downloadPathChanged, widget, &ReplayModItemWidget::setDownloadPath);
        return widget;
    } else
        return nullptr;
}

void ReplayModBrowser::filterList()
{
    auto gameVersion = ui->versionSelect->currentIndex()? GameVersion(ui->versionSelect->currentText()) : GameVersion::Any;
    auto loaderType = ModLoaderType::curseforge.at(ui->loaderSelect->currentIndex());
    auto searchText = ui->searchText->text().toLower();
    for(int row = 0; row < model_->rowCount(); row++){
        auto item = model_->item(row);
        auto mod = item->data().value<ReplayMod*>();
        if(mod){
            setRowHidden(row, (loaderType != ModLoaderType::Any && mod->modInfo().loaderType() != loaderType) ||
                         (gameVersion != GameVersion::Any && gameVersion != mod->modInfo().gameVersion()) ||
                         !(mod->modInfo().name().toLower().contains(searchText) ||
                           mod->modInfo().gameVersionString().contains(searchText)));
        }
    }
}

void ReplayModBrowser::updateStatusText()
{
    auto str = tr("Loaded %1 mods from ReplayMod.").arg(model_->rowCount());
    ui->statusbar->showMessage(str);
}

void ReplayModBrowser::on_actionOpen_Folder_triggered()
{
    QString path;
    if(downloadPath_)
        path = downloadPath_->info().path();
    else
        path = Config().getDownloadPath();
    openFileInFolder(path);
}

