#include "optifinemodbrowser.h"
#include "ui_optifinemodbrowser.h"

#include <QScrollBar>
#include <QDebug>
#include <QStandardItem>
#include <QStatusBar>

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
    model_(new QStandardItemModel(this)),
    api_(new OptifineAPI(this)),
    bmclapi_(new BMCLAPI(this))
{
    ui->setupUi(this);
    ui->menuOptiFine->insertActions(ui->menuOptiFine->actions().first(), menu_->actions());
    initUi(model_);

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
    connect(ui->showPreview, &QCheckBox::stateChanged, this, &OptifineModBrowser::filterList);
    connect(ui->versionSelect, &QComboBox::currentTextChanged, this, &OptifineModBrowser::filterList);
    connect(ui->searchText, &QLineEdit::textChanged, this, &OptifineModBrowser::filterList);

    if(Config().getSearchModsOnStartup()){
        inited_ = true;
        getModList();
    }
}

OptifineModBrowser::~OptifineModBrowser()
{
    for(auto row = 0; row < model_->rowCount(); row++){
        auto mod = model_->index(row, 0).data(Qt::UserRole + 1).value<OptifineMod*>();
        if(mod && !mod->parent())
            mod->deleteLater();
    }
    delete ui;
}

void OptifineModBrowser::load()
{
    if(!inited_){
        inited_ = true;
        getModList();
    }
}

void OptifineModBrowser::refresh()
{
    getModList();
}

void OptifineModBrowser::searchModByPathInfo(LocalModPath *path)
{
    ui->versionSelect->setCurrentText(path->info().gameVersion());
    downloadPathSelectMenu_->setDownloadPath(path);
    filterList();
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

void OptifineModBrowser::filterList()
{
    auto gameVersion = ui->versionSelect->currentIndex()? GameVersion(ui->versionSelect->currentText()) : GameVersion::Any;
    auto showPreview = ui->showPreview->isChecked();
    auto searchText = ui->searchText->text().toLower();
    for(int row = 0; row < model_->rowCount(); row++){
        auto mod = model_->index(row, 0).data(Qt::UserRole + 1).value<OptifineMod*>();
        if(mod){
            setRowHidden(row, (gameVersion != GameVersion::Any && mod->modInfo().gameVersion() != gameVersion) ||
                         (!showPreview && mod->modInfo().isPreview()) ||
                         !(mod->modInfo().name().toLower().contains(searchText) || mod->modInfo().gameVersion().toString().contains(searchText)));
        }
    }
}

void OptifineModBrowser::updateStatusText()
{
    statusBarWidget_->setModCount(model_->rowCount() - 1);
}

void OptifineModBrowser::getModList()
{
    if(!refreshAction_->isEnabled()) return;
    setCursor(Qt::BusyCursor);
    statusBarWidget_->setText(tr("Searching mods..."));
    statusBarWidget_->setProgressVisible(true);
    refreshAction_->setEnabled(false);

    auto source = Config().getOptifineSource();
    if(source == Config::OptifineSourceType::Official)
        searchModsGetter_ = api_->getModList().asUnique();
    else if (source == Config::OptifineSourceType::BMCLAPI)
        searchModsGetter_ = bmclapi_->getOptifineList().asUnique();
    searchModsGetter_->setOnFinished(this, [=](const auto &list){
        setCursor(Qt::ArrowCursor);
        statusBarWidget_->setText("");
        statusBarWidget_->setProgressVisible(false);
        refreshAction_->setEnabled(true);
        for(auto row = 0; row < model_->rowCount(); row++){
            auto item = model_->item(row);
            auto mod = item->data().value<OptifineMod*>();
            if(mod && !mod->parent())
                mod->deleteLater();
        }
        model_->clear();
        QStringList gameVersions;
        for(auto modInfo : list){
            if(!gameVersions.contains(modInfo.gameVersion()))
                gameVersions << modInfo.gameVersion();
            auto mod = new OptifineMod(nullptr, modInfo);
            auto item = new QStandardItem;
            item->setData(QVariant::fromValue(mod));
            model_->appendRow(item);
            item->setSizeHint(QSize(0, 100));
            setRowHidden(item->row(), modInfo.isPreview());
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

QWidget *OptifineModBrowser::getIndexWidget(const QModelIndex &index)
{
    auto mod = index.data(Qt::UserRole + 1).value<OptifineMod*>();
    if(mod){
        auto widget = new OptifineModItemWidget(this, mod);
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
