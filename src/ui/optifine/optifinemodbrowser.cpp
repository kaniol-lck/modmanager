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
    manager_(new OptifineManager(this))
{
    ui->setupUi(this);
    ui->menuOptiFine->insertActions(ui->menuOptiFine->actions().first(), menu_->actions());
    initUi(manager_->model());

    for(auto &&toolBar : findChildren<QToolBar *>())
        ui->menu_View->addAction(toolBar->toggleViewAction());

    //setup status bar
    ui->statusbar->addPermanentWidget(statusBarWidget_);

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
    connect(manager_, &OptifineManager::searchStarted, this, [=]{
        setCursor(Qt::BusyCursor);
        statusBarWidget_->setText(tr("Searching mods..."));
        statusBarWidget_->setProgressVisible(true);
        refreshAction_->setEnabled(false);
    });
    connect(manager_, &OptifineManager::searchFinished, this, [=](bool success){
        setCursor(Qt::ArrowCursor);
        statusBarWidget_->setText(success? "" : tr("Failed loading"));
        statusBarWidget_->setProgressVisible(false);
        refreshAction_->setEnabled(true);
        updateStatusText();
    });
    connect(manager_, &OptifineManager::scrollToTop, this, [=]{
        scrollToTop();
    });

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

void OptifineModBrowser::updateStatusText()
{
    statusBarWidget_->setModCount(manager_->mods().size());
}

QWidget *OptifineModBrowser::getIndexWidget(const QModelIndex &index)
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
