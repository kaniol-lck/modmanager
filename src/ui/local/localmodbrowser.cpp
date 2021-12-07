#include "localmodbrowser.h"
#include "localstatusbarwidget.h"
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
#include <QCheckBox>

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
#include "ui/tagswidget.h"
#include "config.hpp"
#include "util/funcutil.h"
#include "util/localmodsortitem.h"
#include "util/smoothscrollbar.h"
#include "util/unclosedmenu.h"
#include "localmodfilter.h"
#include "localmodmenu.h"
#include "local/localmoditem.h"
#include "ui/curseforge/curseforgemoddialog.h"
#include "ui/curseforge/curseforgemodbrowser.h"
#include "ui/modrinth/modrinthmoddialog.h"
#include "ui/modrinth/modrinthmodbrowser.h"
#include "ui/browserdialog.h"

LocalModBrowser::LocalModBrowser(QWidget *parent, LocalModPath *modPath) :
    Browser(parent),
    ui(new Ui::LocalModBrowser),
    model_(new QStandardItemModel(this)),
    modMenu_(new QMenu(this)),
    infoWidget_(new LocalModInfoWidget(this, modPath)),
    fileListWidget_(new LocalFileListWidget(this)),
    statusBarWidget_(new LocalStatusBarWidget(this)),
    statusBar_(new QStatusBar(this)),
    progressBar_(statusBarWidget_->progressBar()),
    viewSwitcher_(new QButtonGroup(this)),
    modPath_(modPath),
    filter_(new LocalModFilter(this, modPath_))
{
    infoWidget_->hide();
    fileListWidget_->hide();
    ui->setupUi(this);
    //setup mod list
    ui->updateWidget->setVisible(false);
    ui->modListView->setModel(model_);
    ui->modIconListView->setModel(model_);
    ui->modTreeView->setModel(model_);
    ui->modTreeView->hideColumn(0);
    ui->modListView->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->modListView->setProperty("class", "ModList");
    ui->modIconListView->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->modTreeView->setVerticalScrollBar(new SmoothScrollBar(this));

    //setup status bar
    statusBar_->addPermanentWidget(statusBarWidget_);
    connect(statusBarWidget_, &LocalStatusBarWidget::viewModeChanged, ui->stackedWidget, &QStackedWidget::setCurrentIndex);
    ui->mainLayout->addWidget(statusBar_);

    ui->actionReload_Mods->setEnabled(!modPath_->isLoading());

    modMenu_->addAction(ui->actionToggle_Enable);
    modMenu_->addAction(ui->actionToggle_Star);
    modMenu_->addSeparator();
    modMenu_->addAction(ui->actionRename_Selected_Mods);
    modMenu_->addAction(ui->actionRename_to);

    auto findNewMenu = new QMenu(this);
    ui->actionFind_New_Mods->setMenu(findNewMenu);
    findNewMenu->setIcon(QIcon::fromTheme("search"));
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
    ui->openFolderButton->setDefaultAction(ui->actionOpen_Folder);
    ui->findnewButton->setDefaultAction(ui->actionFind_New_Mods);
    ui->findnewButton->setPopupMode(QToolButton::InstantPopup);
    ui->checkUpdatesButton->setDefaultAction(ui->actionCheck_Updates);
    ui->updateAllButton->setDefaultAction(ui->actionUpdate_All);

    connect(filter_->menu(), &UnclosedMenu::menuTriggered, this, &LocalModBrowser::filterList);
    ui->filterButton->setMenu(filter_->menu());
    filter_->showAll();

    auto menu = new QMenu(this);
    menu->addAction(ui->actionReload_Mods);
    menu->addAction(ui->actionBatch_Rename);
    menu->addAction(ui->actionDelete_Old_Files_In_Path);
    menu->addAction(ui->actionLink_Mod_Files);
    ui->menuButton->setMenu(menu);

    auto renameToMenu = new QMenu(this);
    ui->actionRename_to->setMenu(renameToMenu);
    connect(renameToMenu, &QMenu::aboutToShow, this, [=]{
        renameToMenu->clear();
        Config config;
        auto list = config.getRenamePatternHistory();
        for(auto &&v : list){
            auto str = v.toString();
            renameToMenu->addAction(str, this, [=]{
                ModFileRenamer renamer(str);
                renamer.renameMods(selectedMods_);
                Config config;
                auto list = config.getRenamePatternHistory();
                if(list.contains(str))
                    list.removeAll(str);
                list.prepend(str);
                config.setRenamePatternHistory(list);
            });
        }
        if(renameToMenu->isEmpty())
            renameToMenu->addAction(tr("No Rename History"))->setEnabled(false);
        else{
            renameToMenu->addSeparator();
            renameToMenu->addAction(tr("Clear History"), this, [=]{
                Config().setRenamePatternHistory({});
            });
        }
    });

    if(modPath_->modsLoaded())
        updateModList();

    onSelectedModsChanged();

//    onUpdatableCountChanged();

    ui->modTreeView->header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->modTreeView->header(), &QHeaderView::customContextMenuRequested, this, &LocalModBrowser::onModTreeViewHeaderCustomContextMenuRequested);
    connect(ui->modTreeView->header(), &QHeaderView::sectionMoved, this, &LocalModBrowser::saveSections);

    connect(modPath_, &LocalModPath::loadStarted, this, &LocalModBrowser::onLoadStarted);
    connect(modPath_, &LocalModPath::loadProgress, this, &LocalModBrowser::onLoadProgress);
    connect(modPath_, &LocalModPath::loadFinished, this, &LocalModBrowser::onLoadFinished);
    connect(modPath_, &LocalModPath::loadFinished, this, &LocalModBrowser::loadFinished);
    connect(modPath_, &LocalModPath::modListUpdated, this, &LocalModBrowser::updateModList);
    connect(modPath_, &LocalModPath::linkStarted, this, &LocalModBrowser::onLinkStarted);
    connect(modPath_, &LocalModPath::linkProgress, this, &LocalModBrowser::onLinkProgress);
    connect(modPath_, &LocalModPath::linkFinished, this, &LocalModBrowser::onLinkFinished);
    connect(modPath_, &LocalModPath::checkUpdatesStarted, this, &LocalModBrowser::onCheckUpdatesStarted);
    connect(modPath_, &LocalModPath::checkCancelled, this, &LocalModBrowser::onCheckCancelled);
    connect(modPath_, &LocalModPath::updateCheckedCountUpdated, this, &LocalModBrowser::onUpdateCheckedCountUpdated);
    connect(modPath_, &LocalModPath::updatesReady, this, &LocalModBrowser::onUpdatesReady);
    connect(modPath_, &LocalModPath::updatableCountChanged, this, &LocalModBrowser::onUpdatableCountChanged);
    connect(modPath_, &LocalModPath::updatesStarted, this, &LocalModBrowser::onUpdatesStarted);
    connect(modPath_, &LocalModPath::updatesProgress, this, &LocalModBrowser::onUpdatesProgress);
    connect(modPath_, &LocalModPath::updatesDoneCountUpdated, this, &LocalModBrowser::onUpdatesDoneCountUpdated);
    connect(modPath_, &LocalModPath::updatesDone, this, &LocalModBrowser::onUpdatesDone);

    connect(modPath_, &LocalModPath::loadStarted, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_, &LocalModPath::loadProgress, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_, &LocalModPath::loadFinished, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_, &LocalModPath::linkStarted, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_, &LocalModPath::linkProgress, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_, &LocalModPath::linkFinished, this, &LocalModBrowser::updateProgressBar);
//    connect(modPath_, &LocalModPath::checkWebsitesStarted, this, &LocalModBrowser::updateProgressBar);
//    connect(modPath_, &LocalModPath::websiteCheckedCountUpdated, this, &LocalModBrowser::updateProgressBar);
//    connect(modPath_, &LocalModPath::websitesReady, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_, &LocalModPath::checkUpdatesStarted, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_, &LocalModPath::checkCancelled, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_, &LocalModPath::updatableCountChanged, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_, &LocalModPath::updatesReady, this, &LocalModBrowser::updateProgressBar);

    connect(ui->searchText, &QLineEdit::textChanged, this, &LocalModBrowser::filterList);
    connect(ui->modListView, &QListView::doubleClicked, this, &LocalModBrowser::onItemDoubleClicked);
    connect(ui->modIconListView, &QListView::doubleClicked, this, &LocalModBrowser::onItemDoubleClicked);
    connect(ui->modTreeView, &QTreeView::doubleClicked, this, &LocalModBrowser::onItemDoubleClicked);
//    connect(ui->modListView->verticalScrollBar(), &QScrollBar::sliderReleased, this, &LocalModBrowser::updateListViewIndexWidget);
//    connect(ui->modListView->verticalScrollBar(), &QScrollBar::sliderReleased, this, [=]{
//        qDebug() << "release";
//    });
//    connect(ui->modTreeView->verticalScrollBar(), &QScrollBar::sliderReleased, this, &LocalModBrowser::updateTreeViewIndexWidget);

    auto selectionModel = ui->modListView->selectionModel();
    ui->modIconListView->setSelectionModel(selectionModel);
    ui->modTreeView->setSelectionModel(selectionModel);
    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &LocalModBrowser::updateSelectedMods);
    connect(this, &LocalModBrowser::selectedModsChanged, this, &LocalModBrowser::onSelectedModsChanged);
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
    qDebug() << "model clear";
    model_->clear();
    selectedMods_.clear();
    for(auto &&[column, item] : LocalModItem::headerItems())
        model_->setHorizontalHeaderItem(column, item);
    auto list = Config().getLocalModsHeaderSections();
    auto &&header = ui->modTreeView->header();
    if(!list.isEmpty()){
        ui->modTreeView->header()->blockSignals(true);
        for(int i = 0; i < header->count() - 1; i++){
            if(i < list.size())
                header->moveSection(header->visualIndex(list.at(i).toInt()), i + 1);
            else
                header->setSectionHidden(header->logicalIndex(i + 1), true);
        }
        ui->modTreeView->header()->blockSignals(false);
    }

    for(auto &&map : modPath_->modMaps()){
        for (auto &&mod : map) {
            model_->appendRow(LocalModItem::itemsFromMod(mod));
        }
    }
    //TODO: optfine in sub dir
    if(modPath_->info().loaderType() == ModLoaderType::Fabric)
        if(auto mod = modPath_->optiFineMod()){
            model_->appendRow(LocalModItem::itemsFromMod(mod));
        }
    model_->sort(LocalModItem::NameColumn);
    ui->modIconListView->setModelColumn(LocalModItem::NameColumn);
    ui->modTreeView->hideColumn(LocalModItem::ModColumn);
    for(auto &&column : { LocalModItem::EnableColumn, LocalModItem::StarColumn })
        ui->modTreeView->setColumnWidth(column, 0);
    filter_->refreshTags();
    filterList();
}

void LocalModBrowser::updateUi()
{
    for(int i = 0; i < model_->rowCount(); i++){
        if(auto widget = ui->modListView->indexWidget(model_->index(i, 0)))
            dynamic_cast<LocalModItemWidget*>(widget)->updateUi();
    }
}

void LocalModBrowser::onLoadStarted()
{
    ui->actionReload_Mods->setEnabled(false);
    onLoadProgress(0, 0);
    statusBarWidget_->setText(tr("Loading mod files..."));
}

void LocalModBrowser::onLoadProgress(int loadedCount, int totalCount)
{
    statusBarWidget_->setText(tr("Loading mod files.. (Loaded %1/%2 mod files)").arg(loadedCount).arg(totalCount));
    progressBar_->setMaximum(totalCount);
    progressBar_->setValue(loadedCount);
}

void LocalModBrowser::onLoadFinished()
{
    ui->actionReload_Mods->setEnabled(true);
    updateStatusText();
    ui->actionCheck_Updates->setText("Check Updates");
}

void LocalModBrowser::onLinkStarted()
{
    ui->actionLink_Mod_Files->setEnabled(false);
    onLinkProgress(0, 0);
    statusBarWidget_->setText(tr("Linking mod files..."));
}

void LocalModBrowser::onLinkProgress(int linkedCount, int totalCount)
{
    statusBarWidget_->setText(tr("Linkinig mod files.. (Linked %1/%2 mod files)").arg(linkedCount).arg(totalCount));
    progressBar_->setMaximum(totalCount);
    progressBar_->setValue(linkedCount);
}

void LocalModBrowser::onLinkFinished()
{
    ui->actionLink_Mod_Files->setEnabled(true);
    updateStatusText();
}

void LocalModBrowser::onCheckUpdatesStarted()
{
//    ui->checkUpdatesButton->setEnabled(false);
    ui->actionCheck_Updates->setText("Cancel Checking");
    ui->updateWidget->setVisible(false);
    onUpdateCheckedCountUpdated(0, 0, 0);
    progressBar_->setMaximum(modPath_->modCount());
}

void LocalModBrowser::onCheckCancelled()
{
    onUpdatesReady();
}

void LocalModBrowser::onUpdateCheckedCountUpdated(int updateCount, int checkedCount, int totalCount)
{
    if(!modPath_->isChecking()) return;
    statusBarWidget_->setText(tr("%1 mods need update... (Checked %2/%3 mods)").arg(updateCount).arg(checkedCount).arg(totalCount));
    progressBar_->setValue(checkedCount);
}

void LocalModBrowser::onUpdatesReady(int failedCount)
{
    if(modPath_->isUpdating()) return;
//    ui->checkUpdatesButton->setEnabled(true);
    ui->actionCheck_Updates->setText("Check Updates");
    onUpdatableCountChanged();
    updateStatusText();
    if(failedCount)
        QMessageBox::information(this, tr("Update Checking Imcompleted"), tr("%1 mods failed checking update because of network.").arg(failedCount));
}

void LocalModBrowser::onUpdatableCountChanged()
{
    if(modPath_->isChecking()) return;
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
    statusBarWidget_->setText(tr("Updating... (Updated %1/%2 mods)").arg(doneCount).arg(totalCount));
}

void LocalModBrowser::onUpdatesDone(int successCount, int failCount)
{
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
    bool bl = modPath_->isLoading() || modPath_->isLinking() || modPath_->isChecking() || modPath_->isUpdating();
    progressBar_->setVisible(bl);
    if(!bl) statusBarWidget_->setText("");
}

QMenu *LocalModBrowser::getMenu(QList<LocalMod *> mods)
{
    auto menu = new QMenu(this);
    bool isSingleMod = mods.count() == 1;
    if(isSingleMod){
        //single mod
        auto mod = mods.first();
        auto localModMenu = new LocalModMenu(this, mod);
        menu->addAction(QIcon::fromTheme("entry-edit"), tr("Set Alias"), this, [=]{
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
            menu->addAction(tr("Clear update ignores"), this, [=]{
                mod->clearIgnores();
            });
            menu->addSeparator();
        }
        menu->addAction(ui->actionOpen_Mod_Dialog);
        if(mod->curseforgeMod())
            menu->addAction(ui->actionOpen_Curseforge_Mod_Dialog);
        else
            menu->addAction(ui->actionSearch_on_Curseforge);
        if(mod->modrinthMod())
            menu->addAction(ui->actionOpen_Modrinth_Mod_Dialog);
        else
            menu->addAction(ui->actionSearch_on_Modrinth);
        menu->addSeparator();
    }
    //multi mods
    //batch rename
    menu->addAction(ui->actionToggle_Enable);
    menu->addAction(ui->actionToggle_Star);
    menu->addSeparator();
    menu->addAction(ui->actionRename_Selected_Mods);
    menu->addAction(ui->actionRename_to);
    return menu;
}

void LocalModBrowser::updateListViewIndexWidget()
{
    auto beginRow = ui->modListView->indexAt(QPoint(0, 0)).row();
    if(beginRow < 0) return;
    auto endRow = ui->modListView->indexAt(QPoint(0, ui->modListView->height())).row();
    if(endRow < 0)
        endRow = model_->rowCount() - 1;
    else
        //extra 2
        endRow += 2;
    for(int row = beginRow; row <= endRow && row < model_->rowCount(); row++){
        auto index = model_->index(row, 0);
        if(ui->modListView->indexWidget(index)) continue;
//        qDebug() << "new widget at row" << row;
        auto item = model_->item(row);
        if(!item) continue;
        auto mod = item->data().value<LocalMod*>();
        if(mod){
            auto modItemWidget = new LocalModItemWidget(ui->modListView, mod);
            ui->modListView->setIndexWidget(index, modItemWidget);
            item->setSizeHint(QSize(0, modItemWidget->height()));
        }
    }
}

void LocalModBrowser::updateTreeViewIndexWidget()
{
    auto beginRow = ui->modTreeView->indexAt(QPoint(0, 0)).row();
    if(beginRow < 0) return;
    auto endRow = ui->modTreeView->indexAt(QPoint(0, ui->modTreeView->height())).row();
    if(endRow < 0)
        endRow = model_->rowCount() - 1;
    else
        //extra 2
        endRow += 2;
    for(int row = beginRow; row <= endRow && row < model_->rowCount(); row++){
        auto index = model_->index(row, LocalModItem::EnableColumn);
        if(ui->modTreeView->indexWidget(index)) continue;
//        qDebug() << "new widget at row" << row;
        auto item = model_->item(row);
        if(!item) continue;
        auto mod = item->data().value<LocalMod*>();
        if(mod){
            auto enableBox = new QCheckBox(this);
            ui->modTreeView->setIndexWidget(model_->index(row, LocalModItem::EnableColumn), enableBox);
            connect(enableBox, &QCheckBox::toggled, mod, &LocalMod::setEnabled);
            auto starButton = new QToolButton(this);
            starButton->setAutoRaise(true);
            connect(starButton, &QToolButton::clicked, this, [=]{
                auto checked = starButton->icon().name() != "starred-symbolic";
                mod->setFeatured(checked);
            });
            ui->modTreeView->setIndexWidget(model_->index(row, LocalModItem::StarColumn), starButton);
            auto tagsWidget = new TagsWidget(this);
            tagsWidget->setMod(mod);
            ui->modTreeView->setIndexWidget(model_->index(row, LocalModItem::TagsColumn), tagsWidget);
            auto onModChanged = [=]{
                enableBox->setChecked(mod->isEnabled());
                starButton->setIcon(QIcon::fromTheme(mod->isFeatured() ? "starred-symbolic" : "non-starred-symbolic"));
            };
            onModChanged();
            QObject::connect(mod, &LocalMod::modInfoChanged, onModChanged);
        }
    }
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

QList<QAction *> LocalModBrowser::modActions() const
{
    return modMenu_->actions();
}

LocalModPath *LocalModBrowser::modPath() const
{
    return modPath_;
}

void LocalModBrowser::onItemDoubleClicked(const QModelIndex &index)
{
    auto item = model_->itemFromIndex(index.siblingAtColumn(LocalModItem::ModColumn));
    auto mod = item->data().value<LocalMod*>();
    auto dialog = new LocalModDialog(this, mod);
    dialog->show();
}

void LocalModBrowser::on_modListView_customContextMenuRequested(const QPoint &pos)
{
    auto mods = selectedMods(ui->modListView);
    if(auto menu = getMenu(mods); menu && !menu->actions().empty())
        menu->exec(ui->modListView->viewport()->mapToGlobal(pos));
}

void LocalModBrowser::on_modIconListView_customContextMenuRequested(const QPoint &pos)
{
    auto mods = selectedMods(ui->modIconListView);
    if(auto menu = getMenu(mods); menu && !menu->actions().empty())
        menu->exec(ui->modIconListView->viewport()->mapToGlobal(pos));
}

void LocalModBrowser::on_modTreeView_customContextMenuRequested(const QPoint &pos)
{
    auto mods = selectedMods(ui->modTreeView);
    if(auto menu = getMenu(mods); menu && !menu->actions().empty())
        menu->exec(ui->modTreeView->viewport()->mapToGlobal(pos));
}

void LocalModBrowser::onModTreeViewHeaderCustomContextMenuRequested(const QPoint &pos)
{
    auto menu = new QMenu(this);
    for(int column = LocalModItem::ModColumn + 1; column < model_->columnCount(); column++){
        if(column == LocalModItem::NameColumn) continue;
        auto item = model_->horizontalHeaderItem(column);
        auto action = menu->addAction(item->icon(), item->text());
        action->setCheckable(true);
        action->setChecked(!ui->modTreeView->isColumnHidden(column));
        connect(action, &QAction::toggled, this, [=](bool checked){
            ui->modTreeView->setColumnHidden(column, !checked);
            saveSections();
        });
    }
    menu->addSeparator();
    menu->addAction(tr("Reset"), this, [=]{
        for(int column = 1; column < model_->columnCount(); column++){
            auto &&header = ui->modTreeView->header();
            header->moveSection(header->visualIndex(column), column);
            ui->modTreeView->setColumnHidden(column, false);
        }
        Config().setLocalModsHeaderSections(QList<QVariant>());
    });
    menu->exec(ui->modTreeView->mapToGlobal(pos));
}

void LocalModBrowser::saveSections()
{
    auto &&header = ui->modTreeView->header();
    QList<QVariant> list;
    for(auto i = 1; i < header->count(); i++)
        if(!header->isSectionHidden(i)) list << header->logicalIndex(i);
    Config().setLocalModsHeaderSections(list);
}

void LocalModBrowser::paintEvent(QPaintEvent *event)
{
    if(!modPath_->modsLoaded())
        modPath_->loadMods();
    updateListViewIndexWidget();
    updateTreeViewIndexWidget();
    QWidget::paintEvent(event);
}

QAbstractItemView *LocalModBrowser::currentView()
{
    switch(ui->stackedWidget->currentIndex()) {
    case 0:
        return ui->modListView;
    case 1:
        return ui->modIconListView;
    case 2:
        return ui->modTreeView;
    default:
        return nullptr;
    }
}

QList<LocalMod *> LocalModBrowser::selectedMods(QAbstractItemView *view)
{
    if(!view) view = currentView();
    if(!view) return {};
    QList<LocalMod *> list;
    for(auto &&index : view->selectionModel()->selectedRows())
        list << model_->itemFromIndex(index)->data().value<LocalMod*>();
    return list;
}

void LocalModBrowser::on_actionOpen_Folder_triggered()
{
    openFileInFolder(modPath_->info().path());
}

void LocalModBrowser::on_actionBatch_Rename_triggered()
{
    auto dialog = new BatchRenameDialog(this, modPath_);
    dialog->exec();
}

void LocalModBrowser::on_actionDelete_Old_Files_In_Path_triggered()
{
    if(QMessageBox::No == QMessageBox::question(this, tr("Delete"), tr("Delete all old file?"))) return;
    modPath_->deleteAllOld();
}

void LocalModBrowser::on_actionReload_Mods_triggered()
{
    if(!modPath_->isLoading())
        modPath_->loadMods();
}

void LocalModBrowser::on_actionToggle_Enable_triggered(bool checked)
{
    for(auto &&mod : selectedMods_)
        mod->setEnabled(checked);
}

void LocalModBrowser::on_actionToggle_Star_triggered(bool checked)
{
    for(auto &&mod : selectedMods_)
        mod->setFeatured(checked);
}

void LocalModBrowser::on_actionRename_Selected_Mods_triggered()
{
    auto dialog = new BatchRenameDialog(this, selectedMods_);
    dialog->exec();
}

void LocalModBrowser::updateSelectedMods()
{
    if(auto mods = selectedMods(); selectedMods_ != mods){
        selectedMods_ = mods;
        emit selectedModsChanged();
    }
}

void LocalModBrowser::onSelectedModsChanged()
{
    bool noMod = selectedMods_.isEmpty();
    ui->actionToggle_Enable->setEnabled(!noMod);
    ui->actionToggle_Star->setEnabled(!noMod);
    ui->actionRename_Selected_Mods->setEnabled(!noMod);
    ui->actionRename_to->setEnabled(!noMod);

    bool isEnabled = false;
    for(auto &&mod : selectedMods_)
        if(mod->isEnabled()) isEnabled = true;
    ui->actionToggle_Enable->setChecked(isEnabled);
    bool isStarred = false;
    for(auto &&mod : selectedMods_)
        if(mod->isFeatured()) isStarred = true;
    ui->actionToggle_Star->setChecked(isStarred);

    infoWidget_->setMods(selectedMods_);
    fileListWidget_->setMods(selectedMods_);
}

void LocalModBrowser::on_actionOpen_Curseforge_Mod_Dialog_triggered()
{
    if(selectedMods_.count() == 1){
        auto mod = selectedMods_.first();
        if(mod->curseforgeMod()){
            auto dialog = new CurseforgeModDialog(this, mod->curseforgeMod(), mod);
            dialog->show();
        }
    }
}

void LocalModBrowser::on_actionOpen_Modrinth_Mod_Dialog_triggered()
{
    if(selectedMods_.count() == 1){
        auto mod = selectedMods_.first();
        if(mod->modrinthMod()){
            auto dialog = new ModrinthModDialog(this, mod->modrinthMod(), mod);
            dialog->show();
        }
    }
}

void LocalModBrowser::on_actionOpen_Mod_Dialog_triggered()
{
    if(selectedMods_.count() == 1){
        auto mod = selectedMods_.first();
        auto dialog = new LocalModDialog(this, mod);
        dialog->show();
    }
}

void LocalModBrowser::on_actionSearch_on_Curseforge_triggered()
{
    if(selectedMods_.count() == 1){
        auto mod = selectedMods_.first();
        auto browser = new CurseforgeModBrowser(this, mod);
        auto dialog = new BrowserDialog(this, browser);
        dialog->setWindowTitle(tr("Select corresponding mod on Curseforge..."));
        connect(dialog, &BrowserDialog::accepted, this, [=]{
            mod->setCurseforgeMod(browser->selectedMod());
        });
        dialog->exec();
    }
}

void LocalModBrowser::on_actionSearch_on_Modrinth_triggered()
{
    if(selectedMods_.count() == 1){
        auto mod = selectedMods_.first();
        auto browser = new ModrinthModBrowser(this, mod);
        auto dialog = new BrowserDialog(this, browser);
        dialog->setWindowTitle(tr("Select corresponding mod on Modrinth..."));
        connect(dialog, &BrowserDialog::accepted, this, [=]{
            mod->setModrinthMod(browser->selectedMod());
        });
        dialog->exec();
    }
}

void LocalModBrowser::on_actionCheck_Updates_triggered()
{
    if(!modPath()->isChecking())
        modPath_->checkModUpdates();
    else
        modPath_->cancelChecking();
}

void LocalModBrowser::on_actionUpdate_All_triggered()
{
    auto dialog = new LocalModUpdateDialog(this, modPath_);
    dialog->exec();
}


void LocalModBrowser::on_actionLink_Mod_Files_triggered()
{
    modPath_->linkAllFiles();
}

