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
    manager_(new ReplayManager(this))
{
    ui->setupUi(this);
    ui->menuReplayMod->addActions(menu_->actions());
    initUi(manager_);

    for(auto &&toolBar : findChildren<QToolBar *>())
        ui->menu_View->addAction(toolBar->toggleViewAction());

    //setup status bar
    ui->statusbar->addPermanentWidget(statusBarWidget_);

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

void ReplayModBrowser::updateStatusText()
{
    statusBarWidget_->setModCount(manager_->mods().size());
}
