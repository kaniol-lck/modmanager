#include "optifinemodbrowser.h"
#include "ui_optifinemodbrowser.h"

#include <QScrollBar>
#include <QDebug>
#include <QStandardItem>
#include <QStatusBar>

#include "optifine/optifinemanager.h"
#include "ui/downloadpathselectmenu.h"
#include "ui/explorestatusbarwidget.h"
#include "optifinemoditemwidget.h"
#include "optifine/optifineapi.h"
#include "optifine/optifinemod.h"
#include "curseforge/curseforgemod.h"
#include "ui/curseforge/curseforgemoddialog.h"
#include "bmclapi.h"
#include "local/localmodpath.h"
#include "local/localmodpathmanager.h"
#include "util/funcutil.h"
#include "config.hpp"
#include "util/smoothscrollbar.h"

OptifineModBrowser::OptifineModBrowser(QWidget *parent) :
    ExploreBrowser(parent, QIcon(":/image/optifine.png"), "OptiFine", QUrl("https://www.optifine.net")),
    ui(new Ui::OptifineModBrowser),
    manager_(new OptifineManager(this)),
    proxyModel_(new OptifineManagerProxyModel(this))
{
    ui->setupUi(this);
    ui->menuOptiFine->insertActions(ui->menuOptiFine->actions().first(), menu_->actions());
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
    ui->toolBar->addAction(ui->actionGet_OptiFabric);
    ui->toolBar->addAction(ui->actionGet_OptiForge);

    ui->searchBar->addWidget(ui->label);
    ui->searchBar->addWidget(ui->versionSelect);
    ui->searchBar->addWidget(ui->searchText);
    ui->searchBar->addWidget(ui->showPreview);

    updateStatusText();
    if(Config().getSearchModsOnStartup()){
        inited_ = true;
        refresh();
    }
}

OptifineModBrowser::~OptifineModBrowser()
{
    delete ui;
}

void OptifineModBrowser::load()
{
    if(!inited_){
        inited_ = true;
        refresh();
    }
}

void OptifineModBrowser::refresh()
{
    manager_->search();
}

void OptifineModBrowser::searchModByPathInfo(LocalModPath *path)
{
    ui->versionSelect->setCurrentText(path->info().gameVersion());
    downloadPathSelectMenu_->setDownloadPath(path);
}

void OptifineModBrowser::updateUi()
{
//    for(auto &&widget : findChildren<OptifineModItemWidget *>())
//        widget->updateUi();
}

ExploreBrowser *OptifineModBrowser::another()
{
    return new OptifineModBrowser;
}

QWidget *OptifineModBrowser::getListViewIndexWidget(const QModelIndex &index)
{
    auto mod = index.data(Qt::UserRole + 1).value<OptifineMod*>();
    if(mod){
        auto widget = new OptifineModItemWidget(this, mod);
        manager_->model()->setItemHeight(widget->height());
        return widget;
    }else
        return nullptr;
}

void OptifineModBrowser::on_actionGet_OptiFabric_triggered()
{
    //OptiFabric on Curseforge
    //Project ID 322385
    auto mod = new CurseforgeMod(this, 322385);
    auto dialog = new CurseforgeModDialog(this, mod);
    dialog->setDownloadPathSelectMenu(downloadPathSelectMenu_);
    dialog->show();
}

void OptifineModBrowser::on_actionGet_OptiForge_triggered()
{
    //OptiForge on Curseforge
    //Project ID 372196
    auto mod = new CurseforgeMod(this, 372196);
    auto dialog = new CurseforgeModDialog(this, mod);
    dialog->setDownloadPathSelectMenu(downloadPathSelectMenu_);
    dialog->show();
}

void OptifineModBrowser::on_versionSelect_currentIndexChanged(int index)
{
    if(index < 0 || index >= gameVersions_.size()) return;
    proxyModel_->setGameVersion(gameVersions_.at(index));
    proxyModel_->invalidate();
    updateStatusText();
}

void OptifineModBrowser::on_searchText_textChanged(const QString &arg1)
{
    proxyModel_->setText(arg1);
    proxyModel_->invalidate();
    updateStatusText();
}

void OptifineModBrowser::on_showPreview_toggled(bool checked)
{
    proxyModel_->setShowPreview(checked);
    proxyModel_->invalidate();
    updateStatusText();
}
