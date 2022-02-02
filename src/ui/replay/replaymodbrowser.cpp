#include "replaymodbrowser.h"
#include "ui_replaymodbrowser.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QScrollBar>
#include <QStandardItem>
#include <QStatusBar>

#include "replay/replaymanager.h"
#include "ui/downloadpathselectmenu.h"
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
    manager_(new ReplayManager(this)),
    proxyModel_(new ReplayManagerProxyModel(this))
{
    ui->setupUi(this);
    ui->menuReplayMod->addActions(menu_->actions());
    proxyModel_->setSourceModel(manager_->model());
    initUi(manager_, proxyModel_);

    connect(manager_, &ExploreManager::searchFinished, this, [=]{
        gameVersions_.clear();
        gameVersions_ << GameVersion::Any;
        for(auto &&mod : manager_->mods())
            if(auto version = mod->modInfo().gameVersion(); !gameVersions_.contains(version))
                gameVersions_ << version;
        ui->versionSelect->clear();
        for(auto &&version : gameVersions_)
            ui->versionSelect->addItem(version);
    });

    for(auto &&toolBar : findChildren<QToolBar *>())
        ui->menu_View->addAction(toolBar->toggleViewAction());

    ui->toolBar->addMenu(downloadPathSelectMenu_);
    ui->toolBar->addAction(refreshAction_);
    ui->toolBar->addAction(visitWebsiteAction_);
    ui->toolBar->addAction(openDialogAction_);

    ui->searchBar->addWidget(ui->label);
    ui->searchBar->addWidget(ui->versionSelect);
    ui->searchBar->addWidget(ui->label_3);
    ui->searchBar->addWidget(ui->loaderSelect);
    ui->searchBar->addWidget(ui->searchText);

    for(const auto &type : ModLoaderType::replay)
        ui->loaderSelect->addItem(ModLoaderType::icon(type), ModLoaderType::toString(type));

    updateStatusText();

    if(Config().getSearchModsOnStartup()){
        inited_ = true;
        refresh();
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
        refresh();
    }
}

void ReplayModBrowser::refresh()
{
    manager_->search();
}

void ReplayModBrowser::searchModByPathInfo(LocalModPath *path)
{
    ui->versionSelect->setCurrentText(path->info().gameVersion());
    ui->loaderSelect->setCurrentIndex(ModLoaderType::curseforge.indexOf(path->info().loaderType()));
    downloadPathSelectMenu_->setDownloadPath(path);
}

void ReplayModBrowser::updateUi()
{

}

ExploreBrowser *ReplayModBrowser::another()
{
    return new ReplayModBrowser;
}

QWidget *ReplayModBrowser::getIndexWidget(const QModelIndex &index)
{
    auto mod = index.data(Qt::UserRole + 1).value<ReplayMod*>();
    if(mod){
        auto widget = new ReplayModItemWidget(this, mod);
        return widget;
    } else
        return nullptr;
}

void ReplayModBrowser::on_versionSelect_currentIndexChanged(int index)
{
    if(index < 0 || index >= gameVersions_.size()) return;
    proxyModel_->setGameVersion(gameVersions_.at(index));
    proxyModel_->invalidate();
    updateStatusText();
}

void ReplayModBrowser::on_loaderSelect_currentIndexChanged(int index)
{
    proxyModel_->setLoaderType(ModLoaderType::replay.at(index));
    proxyModel_->invalidate();
    updateStatusText();
}

void ReplayModBrowser::on_searchText_textChanged(const QString &arg1)
{
    proxyModel_->setText(arg1);
    proxyModel_->invalidate();
    updateStatusText();
}
