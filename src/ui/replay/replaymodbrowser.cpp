#include "replaymodbrowser.h"
#include "ui_replaymodbrowser.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QScrollBar>
#include <QStandardItem>
#include <QStatusBar>

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
    model_(new QStandardItemModel(this)),
    api_(new ReplayAPI())
{
    ui->setupUi(this);
    ui->menuReplayMod->addActions(menu_->actions());
    initUi(model_);

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

void ReplayModBrowser::searchModByPathInfo(LocalModPath *path)
{
    ui->versionSelect->setCurrentText(path->info().gameVersion());
    ui->loaderSelect->setCurrentIndex(ModLoaderType::curseforge.indexOf(path->info().loaderType()));
    downloadPathSelectMenu_->setDownloadPath(path);
    filterList();
}

void ReplayModBrowser::updateUi()
{

}

ExploreBrowser *ReplayModBrowser::another()
{
    return new ReplayModBrowser;
}

void ReplayModBrowser::getModList()
{
    if(!refreshAction_->isEnabled()) return;
    setCursor(Qt::BusyCursor);
    statusBarWidget_->setText(tr("Searching mods..."));
    statusBarWidget_->setProgressVisible(true);
    refreshAction_->setEnabled(false);

    searchModsGetter_ = api_->getModList().asUnique();
    searchModsGetter_->setOnFinished(this, [=](const auto &list){
        setCursor(Qt::ArrowCursor);
        statusBarWidget_->setText("");
        statusBarWidget_->setProgressVisible(false);
        refreshAction_->setEnabled(true);
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
    }, [=](auto){
        setCursor(Qt::ArrowCursor);
        statusBarWidget_->setText(tr("Failed loading"));
        statusBarWidget_->setProgressVisible(false);
        refreshAction_->setEnabled(true);
    });
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
    statusBarWidget_->setModCount(model_->rowCount() - 1);
}
