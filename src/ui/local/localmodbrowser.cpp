#include "localmodbrowser.h"
#include "ui_localmodbrowser.h"

#include <QMenu>
#include <QMessageBox>
#include <QDebug>
#include <QScrollBar>
#include <QStatusBar>
#include <QProgressBar>
#include <QStandardItemModel>
#include <QTreeView>
#include <QInputDialog>

#include "localmodinfowidget.h"
#include "localfilelistwidget.h"
#include "localmodpathsettingsdialog.h"
#include "local/localmodpath.h"
#include "local/localmodpathmanager.h"
#include "localmoditemwidget.h"
#include "localmoddialog.h"
#include "localmodupdatedialog.h"
#include "localmodcheckdialog.h"
#include "batchrenamedialog.h"
#include "config.hpp"
#include "util/funcutil.h"
#include "util/localmodsortitem.h"
#include "util/smoothscrollbar.h"
#include "util/unclosedmenu.h"
#include "localmodfilter.h"
#include "localmodmenu.h"
#include "local/localmoditem.h"

LocalModBrowser::LocalModBrowser(QWidget *parent, LocalModPath *modPath) :
    Browser(parent),
    ui(new Ui::LocalModBrowser),
    model_(new QStandardItemModel(this)),
    infoWidget_(new LocalModInfoWidget(this)),
    fileListWidget_(new LocalFileListWidget(this)),
    statusBar_(new QStatusBar(this)),
    viewSwitcher_(new QButtonGroup(this)),
    modPath_(modPath),
    filter_(new LocalModFilter(this, modPath_))
{
    ui->setupUi(this);

    //setup mod list
//    ui->updateWidget->setVisible(false);
    ui->modListView->setModel(model_);
    ui->modIconListView->setModel(model_);
    ui->modTreeView->setModel(model_);
    ui->modTreeView->hideColumn(0);
    ui->modListView->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->modListView->setProperty("class", "ModList");
    ui->modIconListView->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->modTreeView->setVerticalScrollBar(new SmoothScrollBar(this));

    //setup status bar
    progressBar_ = new QProgressBar(statusBar_);
    progressBar_->setVisible(false);
    statusBar_->addPermanentWidget(progressBar_);
    auto frame = new QFrame(this);
    frame->setLayout(new QHBoxLayout);
    frame->layout()->setSpacing(0);
    frame->layout()->setMargin(0);
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);
    int id = 0;
    for (auto icon : { "view-list-details", "view-list-icons", "view-list-text" }) {
        auto button = new QToolButton(this);
        button->setCheckable(true);
        button->setAutoRaise(true);
        button->setIcon(QIcon::fromTheme(icon));
        viewSwitcher_->addButton(button, id++);
        frame->layout()->addWidget(button);
    }
    statusBar_->addPermanentWidget(frame);
    ui->mainLayout->addWidget(statusBar_);

    viewSwitcher_->button(0)->setChecked(true);
    connect(viewSwitcher_, &QButtonGroup::idClicked, ui->stackedWidget, &QStackedWidget::setCurrentIndex);

    auto findNewMenu = new QMenu(this);
    connect(findNewMenu->addAction(QIcon(":/image/curseforge.svg"), "Curseforge"), &QAction::triggered, this, [=]{
        emit findNewOnCurseforge(modPath_->info());
    });
    connect(findNewMenu->addAction(QIcon(":/image/modrinth.svg"), "Modrinth"), &QAction::triggered, this, [=]{
        emit findNewOnModrinth(modPath_->info());
    });
    connect(findNewMenu->addAction(QIcon(":/image/optifine.png"), "OptiFine"), &QAction::triggered, this, [=]{
        emit findNewOnOptifine(modPath_->info());
    });
    connect(findNewMenu->addAction(QIcon(":/image/replay.png"), "ReplayMod"), &QAction::triggered, this, [=]{
        emit findNewOnReplay(modPath_->info());
    });
    ui->findnewButton->setMenu(findNewMenu);

    connect(filter_->menu(), &UnclosedMenu::menuTriggered, this, &LocalModBrowser::filterList);
    ui->filterButton->setMenu(filter_->menu());
    filter_->showAll();

    auto menu = new QMenu(this);
//    connect(menu->addAction(QIcon::fromTheme("entry-edit"), tr("Edit")), &QAction::triggered, this, [=]{
//        auto pathInfo = modPath_->info();
//        auto dialog = new LocalModPathSettingsDialog(this, pathInfo);
//        connect(dialog, &LocalModPathSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &newInfo){
//            modPath_->setInfo(newInfo);
//            localItem_->child(index)->setText(0, newInfo.displayName());
//        });
//        dialog->exec();
//    });
    auto reloadAction = menu->addAction(QIcon::fromTheme("view-refresh"), tr("Reload"));
    connect(menu, &QMenu::aboutToShow, this, [=]{
        if(isLoading()){
            reloadAction->setEnabled(false);
            connect(this, &LocalModBrowser::loadFinished, this, [=]{
                reloadAction->setEnabled(true);
            });
        }
    });
    connect(reloadAction, &QAction::triggered, this, [=]{
        modPath_->loadMods();
    });
//    connect(menu->addAction(QIcon::fromTheme("delete"), tr("Delete")), &QAction::triggered, this, [=]{
//        if(QMessageBox::No == QMessageBox::question(this, tr("Delete"), tr("Delete this mod path?"))) return;
//        LocalModPathManager::removePath(modPath_);
//    });
//    menu->addSeparator();
    connect(menu->addAction(QIcon::fromTheme("entry-edit"), tr("Batch rename")), &QAction::triggered, this, [=]{
        auto dialog = new BatchRenameDialog(this, modPath_);
        dialog->exec();
    });
    connect(menu->addAction(QIcon::fromTheme("delete"), tr("Delete old")), &QAction::triggered, this, [=]{
        if(QMessageBox::No == QMessageBox::question(this, tr("Delete"), tr("Delete all old file?"))) return;
        modPath_->deleteAllOld();
    });
    ui->menuButton->setMenu(menu);

    updateModList();

    connect(modPath_, &LocalModPath::loadStarted, this, &LocalModBrowser::onLoadStarted);
    connect(modPath_, &LocalModPath::loadProgress, this, &LocalModBrowser::onLoadProgress);
    connect(modPath_, &LocalModPath::loadFinished, this, &LocalModBrowser::onLoadFinished);
    connect(modPath_, &LocalModPath::loadFinished, this, &LocalModBrowser::loadFinished);
    connect(modPath_, &LocalModPath::modListUpdated, this, &LocalModBrowser::updateModList);
    connect(modPath_, &LocalModPath::websiteCheckedCountUpdated, this, &LocalModBrowser::onWebsiteCheckedCountUpdated);
    connect(modPath_, &LocalModPath::checkWebsitesStarted, this, &LocalModBrowser::onCheckWebsitesStarted);
    connect(modPath_, &LocalModPath::websitesReady, this, &LocalModBrowser::onWebsitesReady);
    connect(modPath_, &LocalModPath::checkUpdatesStarted, this, &LocalModBrowser::onCheckUpdatesStarted);
    connect(modPath_, &LocalModPath::updateCheckedCountUpdated, this, &LocalModBrowser::onUpdateCheckedCountUpdated);
    connect(modPath_, &LocalModPath::updatesReady, this, &LocalModBrowser::onUpdatesReady);
    connect(modPath_, &LocalModPath::updatableCountChanged, this, &LocalModBrowser::onUpdatableCountChanged);
    connect(modPath_, &LocalModPath::updatesStarted, this, &LocalModBrowser::onUpdatesStarted);
    connect(modPath_, &LocalModPath::updatesProgress, this, &LocalModBrowser::onUpdatesProgress);
    connect(modPath_, &LocalModPath::updatesDoneCountUpdated, this, &LocalModBrowser::onUpdatesDoneCountUpdated);
    connect(modPath_, &LocalModPath::updatesDone, this, &LocalModBrowser::onUpdatesDone);

    connect(modPath_, &LocalModPath::loadStarted, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_, &LocalModPath::loadProgress, this, &LocalModBrowser::onLoadProgress);
    connect(modPath_, &LocalModPath::loadFinished, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_, &LocalModPath::checkWebsitesStarted, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_, &LocalModPath::websiteCheckedCountUpdated, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_, &LocalModPath::websitesReady, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_, &LocalModPath::checkUpdatesStarted, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_, &LocalModPath::updatableCountChanged, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_, &LocalModPath::updatesReady, this, &LocalModBrowser::updateProgressBar);

    connect(ui->searchText, &QLineEdit::textChanged, this, &LocalModBrowser::filterList);

    connect(ui->modListView, &QListView::entered, this, &LocalModBrowser::onItemSelected);
    connect(ui->modListView, &QListView::clicked, this, &LocalModBrowser::onItemSelected);
    connect(ui->modIconListView, &QListView::entered, this, &LocalModBrowser::onItemSelected);
    connect(ui->modIconListView, &QListView::clicked, this, &LocalModBrowser::onItemSelected);
    connect(ui->modTreeView, &QTreeView::entered, this, &LocalModBrowser::onItemSelected);
    connect(ui->modTreeView, &QTreeView::clicked, this, &LocalModBrowser::onItemSelected);

    connect(ui->modListView, &QListView::doubleClicked, this, &LocalModBrowser::onItemDoubleClicked);
    connect(ui->modIconListView, &QListView::doubleClicked, this, &LocalModBrowser::onItemDoubleClicked);
    connect(ui->modTreeView, &QTreeView::doubleClicked, this, &LocalModBrowser::onItemDoubleClicked);
}

LocalModBrowser::~LocalModBrowser()
{
    delete ui;
}

bool LocalModBrowser::isLoading() const
{
    return modPath_->isLoading();
}

void LocalModBrowser::reload()
{
    if(modPath_->isLoading()) return;
    modPath_->loadMods();
}

void LocalModBrowser::updateModList()
{
    model_->clear();
    model_->setHorizontalHeaderItem(NameColumn, new QStandardItem(tr("Mod Name")));
    model_->setHorizontalHeaderItem(IdColumn, new QStandardItem(tr("ID")));
    model_->setHorizontalHeaderItem(VersionColumn, new QStandardItem(tr("Version")));
    model_->setHorizontalHeaderItem(FileDateColumn, new QStandardItem(tr("Last Modified")));
    model_->setHorizontalHeaderItem(FileSizeColumn, new QStandardItem(tr("File Size")));
    model_->setHorizontalHeaderItem(CurseforgeIdColumn, new QStandardItem(tr("Curseforge Id")));
    model_->setHorizontalHeaderItem(ModrinthIdColumn, new QStandardItem(tr("Modrinth Id")));
    model_->setHorizontalHeaderItem(DescriptionColumn, new QStandardItem(tr("Description")));

    for(auto &&map : modPath_->modMaps()){
        for (auto &&mod : map) {
            auto items = LocalModItem::itemsFromMod(mod);
            model_->appendRow(items);
            auto modItemWidget = new LocalModItemWidget(ui->modListView, mod);
            ui->modListView->setIndexWidget(model_->indexFromItem(items.first()), modItemWidget);
            items.first()->setSizeHint(QSize(0, modItemWidget->height()));
        }
    }
    //TODO: optfine in sub dir
    if(modPath_->info().loaderType() == ModLoaderType::Fabric)
        if(auto mod = modPath_->optiFineMod()){
            auto items = LocalModItem::itemsFromMod(mod);
            model_->appendRow(items);
            auto modItemWidget = new LocalModItemWidget(ui->modListView, mod);
            ui->modListView->setIndexWidget(model_->indexFromItem(items.first()), modItemWidget);
            items.first()->setSizeHint(QSize(0, modItemWidget->height()));
        }
    model_->sort(NameColumn);
    ui->modIconListView->setModelColumn(NameColumn);
    ui->modTreeView->hideColumn(ModColumn);
    filter_->refreshTags();
    filterList();
}

void LocalModBrowser::updateUi()
{
    for(int i = 0; i < model_->rowCount(); i++){
        auto widget = ui->modListView->indexWidget(model_->index(i, 0));
        dynamic_cast<LocalModItemWidget*>(widget)->updateUi();
    }
}

void LocalModBrowser::onLoadStarted()
{
    onLoadProgress(0, 0);
    statusBar_->showMessage(tr("Loading mod files..."));
}

void LocalModBrowser::onLoadProgress(int loadedCount, int totalCount)
{
    statusBar_->showMessage(tr("Loading mod files.. (Loaded %1/%2 mod files)").arg(loadedCount).arg(totalCount));
    progressBar_->setMaximum(totalCount);
    progressBar_->setValue(loadedCount);
}

void LocalModBrowser::onLoadFinished()
{
    updateStatusText();
}

void LocalModBrowser::onCheckWebsitesStarted()
{
    ui->checkUpdatesButton->setEnabled(false);
    onWebsiteCheckedCountUpdated(0);
    ui->checkUpdatesButton->setText(tr("Searching on mod websites..."));
    progressBar_->setMaximum(modPath_->modCount());
}

void LocalModBrowser::onWebsiteCheckedCountUpdated(int checkedCount)
{
    statusBar_->showMessage(tr("Searching on mod websites... (Searched %1/%2 mods)").arg(checkedCount).arg(modPath_->modCount()));
    progressBar_->setValue(checkedCount);
}

void LocalModBrowser::onWebsitesReady()
{
    ui->checkUpdatesButton->setEnabled(true);
    ui->checkUpdatesButton->setText(tr("Check updates"));
    updateStatusText();
}

void LocalModBrowser::onCheckUpdatesStarted()
{
    isChecking_ = true;
    ui->checkUpdatesButton->setEnabled(false);
    ui->updateWidget->setVisible(false);
    onUpdateCheckedCountUpdated(0, 0, 0);
    ui->checkUpdatesButton->setText(tr("Checking updates..."));
    progressBar_->setMaximum(modPath_->modCount());
}

void LocalModBrowser::onUpdateCheckedCountUpdated(int updateCount, int checkedCount, int totalCount)
{
    if(!isChecking_) return;
    statusBar_->showMessage(tr("%1 mods need update... (Checked %2/%3 mods)").arg(updateCount).arg(checkedCount).arg(totalCount));
    progressBar_->setValue(checkedCount);
}

void LocalModBrowser::onUpdatesReady(int failedCount)
{
    isChecking_ = false;
    if(isUpdating_) return;
    ui->checkUpdatesButton->setEnabled(true);
    ui->checkUpdatesButton->setText(tr("Check updates"));
    onUpdatableCountChanged();
    updateStatusText();
    if(failedCount)
        QMessageBox::information(this, tr("Update Checking Imcompleted"), tr("%1 mods failed checking update because of network.").arg(failedCount));
}

void LocalModBrowser::onUpdatableCountChanged()
{
    if(isChecking_) return;
    if(auto count = modPath_->updatableCount()){
        ui->updateWidget->setVisible(true);
        ui->updateAllButton->setVisible(true);
        ui->updateAllButton->setEnabled(true);
        ui->updateAllButton->setText(tr("Update All"));
        ui->updateProgressText->setText(tr("%1 mods need update.").arg(count));
    } else {
        ui->updateWidget->setVisible(false);
    }
}

void LocalModBrowser::onUpdatesStarted()
{
    isUpdating_ = true;
    ui->updateAllButton->setEnabled(false);
    onUpdatesProgress(0, 0);
    ui->updateAllButton->setText(tr("Updating..."));
}

void LocalModBrowser::onUpdatesProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    progressBar_->setValue(bytesReceived);
    progressBar_->setMaximum(bytesTotal);
}

void LocalModBrowser::onUpdatesDoneCountUpdated(int doneCount, int totalCount)
{
    statusBar_->showMessage(tr("Updating... (Updated %1/%2 mods)").arg(doneCount).arg(totalCount));
}

void LocalModBrowser::onUpdatesDone(int successCount, int failCount)
{
    isUpdating_ = false;
    auto str = tr("%1 mods in %2 has been updated. Enjoy it!").arg(successCount).arg(modPath_->info().displayName());
    if(failCount)
        str.append("\n").append(tr("Sadly, %1 mods failed to update.").arg(failCount));
    if(Config().getPostUpdate() == Config::Keep)
        str.append("\n").append(tr("You can revert update if find any incompatibility."));
    QMessageBox::information(this, tr("Update Finished"), str);

    onUpdatesReady();
}

void LocalModBrowser::filterList()
{
    hiddenCount_ = 0;
    for(int i = 0; i < model_->rowCount(); i++){
        auto item = model_->item(i);
        auto mod = item->data().value<LocalMod*>();
        auto hidden = !filter_->willShow(mod, ui->searchText->text().toLower());
        ui->modListView->setRowHidden(i, hidden);
        ui->modIconListView->setRowHidden(i, hidden);
        ui->modTreeView->setRowHidden(i, ui->modTreeView->rootIndex(), hidden);
        if(hidden) hiddenCount_++;
    }
    updateStatusText();
}

void LocalModBrowser::updateStatusText()
{
    auto str = tr("%1 mods in total. ").arg(modPath_->modCount());
    if(hiddenCount_){
        if(hiddenCount_ < modPath_->modCount() / 2)
            str.append(tr("(%1 mods are hidden)").arg(hiddenCount_));
        else
            str.append(tr("(%1 mods are shown)").arg(modPath_->modCount() - hiddenCount_));
    }
    statusBar_->showMessage(str);
}

void LocalModBrowser::updateProgressBar()
{
    progressBar_->setVisible(modPath_->isLoading() || modPath_->isSearching() || modPath_->isChecking());
}

QMenu *LocalModBrowser::onCustomContextMenuRequested(const QModelIndex &index)
{
    if(!index.isValid()) return nullptr;
    auto menu = new QMenu(this);
    auto mod = model_->itemFromIndex(index.siblingAtColumn(ModColumn))->data().value<LocalMod*>();
    auto localModMenu = new LocalModMenu(this, mod);
    connect(menu->addAction(QIcon::fromTheme("entry-edit"), tr("Set Alias")), &QAction::triggered, this, [=]{
        bool ok;
        auto alias = QInputDialog::getText(this, tr("Set mod alias"), tr("Alias of <b>%1</b> mod:").arg(mod->commonInfo()->name()), QLineEdit::Normal, mod->alias(), &ok);
        if(ok)
            mod->setAlias(alias);
    });
    menu->addMenu(localModMenu->addTagMenu());
    if(!mod->customizableTags().isEmpty())
        menu->addMenu(localModMenu->removeTagmenu());
    menu->addSeparator();
    if(!mod->curseforgeUpdate().ignores().isEmpty() || !mod->modrinthUpdate().ignores().isEmpty()){
        connect(menu->addAction(tr("Clear update ignores")), &QAction::triggered, this, [=]{
            mod->clearIgnores();
        });
        menu->addSeparator();
    }
    auto starAction = menu->addAction(QIcon::fromTheme("non-starred-symbolic"), tr("Star"));
    starAction->setCheckable(true);
    starAction->setChecked(mod->isFeatured());
    connect(starAction, &QAction::toggled, this, [=](bool bl){
        mod->setFeatured(bl);
    });
    auto disableAction = menu->addAction(QIcon::fromTheme("action-unavailable-symbolic"), tr("Disable"));
    disableAction->setCheckable(true);
    disableAction->setChecked(mod->isDisabled());
    connect(disableAction, &QAction::toggled, this, [=](bool bl){
        mod->setEnabled(!bl);
    });
    return menu;
}

void LocalModBrowser::on_comboBox_currentIndexChanged(int index)
{
//    for(int i = 0; i < ui->modListView->count(); i++){
//        auto item = dynamic_cast<LocalModSortItem*>(ui->modListView->item(i));
//        item->setSortRule(static_cast<LocalModSortItem::SortRule>(index));
//    }
//    ui->modListView->sortItems();
}

void LocalModBrowser::on_checkUpdatesButton_clicked()
{
    modPath_->checkModUpdates();
}

void LocalModBrowser::on_openFolderButton_clicked()
{
    openFileInFolder(modPath_->info().path());
}

void LocalModBrowser::on_checkButton_clicked()
{
    auto dialog = new LocalModCheckDialog(this, modPath_);
    dialog->exec();
}

void LocalModBrowser::on_updateAllButton_clicked()
{
    auto dialog = new LocalModUpdateDialog(this, modPath_);
    dialog->exec();
}

void LocalModBrowser::on_hideUpdatesButton_clicked()
{
    ui->updateWidget->setVisible(false);
}

QWidget *LocalModBrowser::infoWidget() const
{
    return infoWidget_;
}

QWidget *LocalModBrowser::fileListWidget() const
{
    return fileListWidget_;
}

LocalModPath *LocalModBrowser::modPath() const
{
    return modPath_;
}

void LocalModBrowser::onItemSelected(const QModelIndex &index)
{
    auto item = model_->itemFromIndex(index.siblingAtColumn(ModColumn));
    auto mod = item->data().value<LocalMod*>();
    infoWidget_->setMod(mod);
    fileListWidget_->setMod(mod);
}

void LocalModBrowser::onItemDoubleClicked(const QModelIndex &index)
{
    auto item = model_->itemFromIndex(index.siblingAtColumn(ModColumn));
    auto mod = item->data().value<LocalMod*>();
    auto dialog = new LocalModDialog(this, mod);
    dialog->show();
}

void LocalModBrowser::on_modListView_customContextMenuRequested(const QPoint &pos)
{
    auto index = ui->modListView->indexAt(pos);
    if(auto menu = onCustomContextMenuRequested(index); menu && !menu->actions().empty())
        menu->exec(ui->modListView->mapToGlobal(pos));
}

void LocalModBrowser::on_modIconListView_customContextMenuRequested(const QPoint &pos)
{
    auto index = ui->modIconListView->indexAt(pos);
    if(auto menu = onCustomContextMenuRequested(index); menu && !menu->actions().empty())
        menu->exec(ui->modIconListView->mapToGlobal(pos));
}

void LocalModBrowser::on_modTreeView_customContextMenuRequested(const QPoint &pos)
{
    auto index = ui->modTreeView->indexAt(pos);
    if(auto menu = onCustomContextMenuRequested(index); menu && !menu->actions().empty())
        menu->exec(ui->modTreeView->mapToGlobal(pos));
}
