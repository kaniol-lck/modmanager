#include "localmodbrowser.h"
#include "localstatusbarwidget.h"
#include "ui_localmodbrowser.h"

#include <QMenu>
#include <QMessageBox>
#include <QDebug>
#include <QScrollBar>
#include <QProgressBar>
#include <QStandardItemModel>
#include <QTreeView>
#include <QInputDialog>
#include <QCheckBox>
#include <QActionGroup>
#include <QFileDialog>
#include <JlCompress.h>
#include <QtConcurrent>

#include "localmodinfowidget.h"
#include "localfilelistwidget.h"
#include "localmodpathsettingsdialog.h"
#include "local/localmodpath.h"
#include "local/localmodpathmanager.h"
#include "localmoditemwidget.h"
#include "localmoddialog.h"
#include "localmodupdatedialog.h"
#include "localmodcheckdialog.h"
#include "exportmanifestjsondialog.h"
#include "batchrenamedialog.h"
#include "ui/tagswidget.h"
#include "config.hpp"
#include "util/funcutil.h"
#include "util/localmodsortitem.h"
#include "util/smoothscrollbar.h"
#include "util/unclosedmenu.h"
#include "localmodfilter.h"
#include "localmodmenu.h"
#include "ui/local/localmodpathmodel.h"
#include "ui/curseforge/curseforgemoddialog.h"
#include "ui/curseforge/curseforgemodbrowser.h"
#include "ui/modrinth/modrinthmoddialog.h"
#include "ui/modrinth/modrinthmodbrowser.h"
#include "ui/github/githubrepobrowser.h"
#include "ui/browserdialog.h"
#include "util/checksheet.h"
#include "ui/pageswitcher.h"

LocalModBrowser::LocalModBrowser(QWidget *parent, LocalModPath *modPath) :
    Browser(parent),
    ui(new Ui::LocalModBrowser),
    model_(new LocalModPathModel(modPath)),
    proxyModel_(new LocalModPathFilterProxyModel(this)),
    infoWidget_(new LocalModInfoWidget(this, modPath)),
    fileListWidget_(new LocalFileListWidget(this, modPath)),
    statusBarWidget_(new LocalStatusBarWidget(this)),
    progressBar_(statusBarWidget_->progressBar()),
    viewSwitcher_(new QButtonGroup(this)),
    modPath_(modPath),
    filter_(new LocalModFilter(this, modPath_))
{
    infoWidget_->hide();
    fileListWidget_->hide();
    ui->setupUi(this);
    Config config;
    restoreGeometry(config.getBrowserWindowState(this));
    restoreState(config.getBrowserWindowState(this));
    //setup mod list
    proxyModel_->setSourceModel(model_);
    proxyModel_->setFilter(filter_);
    ui->updateWidget->setVisible(false);
    ui->modListView->setModel(proxyModel_);
    ui->modIconListView->setModel(proxyModel_);
    ui->modTreeView->setModel(proxyModel_);
    ui->modTreeView->hideColumn(0);
    ui->modTreeView->hideColumn(0);
    ui->modIconListView->setModelColumn(LocalModPathModel::NameColumn);
    ui->modListView->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->modListView->setProperty("class", "ModList");
    ui->modIconListView->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->modTreeView->setVerticalScrollBar(new SmoothScrollBar(this));

    for(auto &&toolBar : findChildren<QToolBar *>())
        ui->menu_View->addAction(toolBar->toggleViewAction());

    ui->toolBar->insertMenu(ui->actionCheck_Updates, ui->actionFind_New_Mods);
    ui->searchBar->addWidget(ui->searchText);
    ui->searchBar->addMenu(ui->actionFilter);

    //setup status bar
    ui->statusBar->addPermanentWidget(statusBarWidget_);
    connect(statusBarWidget_, &LocalStatusBarWidget::viewModeChanged, ui->stackedWidget, &QStackedWidget::setCurrentIndex);

    ui->actionReload_Mods->setEnabled(!modPath_->isLoading());

    auto sortMenu = new QMenu(this);
    ui->actionSort->setMenu(sortMenu);
    sortMenu->setIcon(QIcon::fromTheme("view-sort"));
    connect(sortMenu, &QMenu::aboutToShow, this, [=]{
        sortMenu->clear();
        auto order = ui->modTreeView->header()->sortIndicatorOrder();
        auto section = ui->modTreeView->header()->sortIndicatorSection();
        auto orderGroup = new QActionGroup(this);
        auto sectionGroup = new QActionGroup(this);
        sortMenu->addSection(tr("Order"));
        auto ascendingAction = sortMenu->addAction(QIcon::fromTheme("view-sort-ascending"), tr("Ascending"), this, [=]{
            ui->modTreeView->sortByColumn(section, Qt::AscendingOrder);
        });
        auto descendingAction = sortMenu->addAction(QIcon::fromTheme("view-sort-descending"), tr("Descending"), this, [=]{
            ui->modTreeView->sortByColumn(section, Qt::DescendingOrder);
        });
        ascendingAction->setCheckable(true);
        descendingAction->setCheckable(true);
        (order == Qt::AscendingOrder? ascendingAction : descendingAction)->setChecked(true);
        orderGroup->addAction(ascendingAction);
        orderGroup->addAction(descendingAction);
        sortMenu->addSection(tr("Column"));
        for(int column = LocalModPathModel::ModColumn + 1; column < model_->columnCount(); column++){
            auto text = model_->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();
            auto icon = model_->headerData(column, Qt::Horizontal, Qt::DecorationRole).value<QIcon>();
            auto action = sortMenu->addAction(icon, text, this, [=]{
                ui->modTreeView->sortByColumn(column, order);
            });
            action->setCheckable(true);
            sectionGroup->addAction(action);
            if(column == section)
                action->setChecked(true);
        }
    });

    auto findNewMenu = new QMenu(this);
    ui->actionFind_New_Mods->setMenu(findNewMenu);
    findNewMenu->setIcon(QIcon::fromTheme("search"));
    connect(findNewMenu, &QMenu::aboutToShow, this, [=]{
        findNewMenu->clear();
        if(!pageSwitcher_) return;
        auto exploreItem = pageSwitcher_->model()->item(PageSwitcher::Explore);
        for(int row = 0; row < exploreItem->rowCount(); row++){
            auto item = exploreItem->child(row);
            auto browser = item->data().value<ExploreBrowser *>();
            findNewMenu->addAction(browser->icon(), browser->name(), [=]{
                pageSwitcher_->setPage(PageSwitcher::Explore, row);
                browser->searchModByPathInfo(modPath_);
            });
        }
    });
    ui->updateAllButton->setDefaultAction(ui->actionUpdate_All);

    connect(filter_->menu(), &UnclosedMenu::menuTriggered, proxyModel_, &QSortFilterProxyModel::invalidate);
    connect(filter_->menu(), &UnclosedMenu::menuTriggered, this, &LocalModBrowser::updateStatusText);
    connect(filter_->menu(), &UnclosedMenu::menuTriggered, this, &LocalModBrowser::updateListViewIndexWidget);
    connect(filter_->menu(), &UnclosedMenu::menuTriggered, this, &LocalModBrowser::updateTreeViewIndexWidget);
    ui->actionFilter->setMenu(filter_->menu());
    filter_->showAll();

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
            renameToMenu->addAction(QIcon::fromTheme("edit-clear-history"), tr("Clear History"), this, [=]{
                Config().setRenamePatternHistory({});
            });
        }
    });

    auto moveToMenu = new QMenu(this);
    ui->actionMove_Selected_Mods_to->setMenu(moveToMenu);
    auto addMoveToAction = [=](LocalModPath *path){
        for(const auto &mod : qAsConst(selectedMods_))
            if(mod->path() == path) return;
        moveToMenu->addAction(path->relative().isEmpty()? tr("Main Folder") : path->relative().join("/"), this, [=]{
            for(const auto &mod : qAsConst(selectedMods_)){
                mod->moveTo(path);
            }
        });
    };
    connect(moveToMenu, &QMenu::aboutToShow, this, [=]{
        moveToMenu->clear();
        addMoveToAction(modPath_);
        moveToMenu->addSection(tr("Sub Directories"));
        for(const auto &subPath : modPath_->subPaths()){
            addMoveToAction(subPath);
        }
        moveToMenu->addSeparator();
        moveToMenu->addAction(tr("New Sub-Directory..."), this, [=]{
            auto pathName = QInputDialog::getText(this, tr("New Sub-Directory"), tr("Sub-Directory Name:"));
            if(pathName.isEmpty()) return;
            auto subPath = modPath_->addSubPath(pathName);
            addMoveToAction(subPath);
        });
    });

    if(modPath_->modsLoaded())
        filter_->refreshTags();

    onSelectedModsChanged();
    onUpdatableCountChanged();

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

    ui->modTreeView->header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->modTreeView->header(), &QHeaderView::customContextMenuRequested, this, &LocalModBrowser::onModTreeViewHeaderCustomContextMenuRequested);
    connect(ui->modTreeView->header(), &QHeaderView::sectionMoved, this, &LocalModBrowser::saveSections);

    connect(modPath_, &LocalModPath::loadStarted, this, &LocalModBrowser::onLoadStarted);
    connect(modPath_, &LocalModPath::loadProgress, this, &LocalModBrowser::onLoadProgress);
    connect(modPath_, &LocalModPath::loadFinished, this, &LocalModBrowser::onLoadFinished);
    connect(modPath_, &LocalModPath::loadFinished, this, &LocalModBrowser::loadFinished);
    connect(modPath_, &LocalModPath::modListUpdated, filter_, &LocalModFilter::refreshTags);
    connect(modPath_, &LocalModPath::modListUpdated, proxyModel_, &QSortFilterProxyModel::invalidate);
    connect(modPath_, &LocalModPath::modListUpdated, this, &LocalModBrowser::updateStatusText);
    connect(modPath_, &LocalModPath::modListUpdated, this, [=]{
        ui->modTreeView->sortByColumn(LocalModPathModel::NameColumn, Qt::AscendingOrder);
    });
    connect(modPath_->modsLinker(), &CheckSheet::started, this, &LocalModBrowser::onLinkStarted);
    connect(modPath_->modsLinker(), &CheckSheet::progress, this, &LocalModBrowser::onLinkProgress);
    connect(modPath_->modsLinker(), &CheckSheet::finished, this, &LocalModBrowser::onLinkFinished);
    connect(modPath_->updateChecker(), &CheckSheet::started, this, &LocalModBrowser::onCheckUpdatesStarted);
    connect(modPath_, &LocalModPath::checkCancelled, this, &LocalModBrowser::onCheckCancelled);
    connect(modPath_->updateChecker(), &CheckSheet::progress, this, &LocalModBrowser::onUpdateCheckedCountUpdated);
    connect(modPath_->updateChecker(), &CheckSheet::finished, this, &LocalModBrowser::onCheckUpdatesFinished);
    connect(modPath_, &LocalModPath::updatableCountChanged, this, &LocalModBrowser::onUpdatableCountChanged);
    connect(modPath_, &LocalModPath::updatesStarted, this, &LocalModBrowser::onUpdatesStarted);
    connect(modPath_, &LocalModPath::updatesProgress, this, &LocalModBrowser::onUpdatesProgress);
    connect(modPath_, &LocalModPath::updatesDoneCountUpdated, this, &LocalModBrowser::onUpdatesDoneCountUpdated);
    connect(modPath_, &LocalModPath::updatesDone, this, &LocalModBrowser::onUpdatesDone);

    connect(modPath_, &LocalModPath::loadStarted, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_, &LocalModPath::loadProgress, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_, &LocalModPath::loadFinished, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_->modsLinker(), &CheckSheet::started, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_->modsLinker(), &CheckSheet::progress, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_->modsLinker(), &CheckSheet::finished, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_->updateChecker(), &CheckSheet::started, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_, &LocalModPath::checkCancelled, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_->updateChecker(), &CheckSheet::progress, this, &LocalModBrowser::updateProgressBar);
    connect(modPath_->updateChecker(), &CheckSheet::finished, this, &LocalModBrowser::updateProgressBar);

    connect(ui->searchText, &QLineEdit::textChanged, this, [=]{
        proxyModel_->setText(ui->searchText->text().toLower());
        proxyModel_->invalidate();
        updateStatusText();
    });
    connect(ui->modListView, &QListView::doubleClicked, this, &LocalModBrowser::onItemDoubleClicked);
    connect(ui->modIconListView, &QListView::doubleClicked, this, &LocalModBrowser::onItemDoubleClicked);
    connect(ui->modTreeView, &QTreeView::doubleClicked, this, &LocalModBrowser::onItemDoubleClicked);

    auto selectionModel = ui->modListView->selectionModel();
    ui->modIconListView->setSelectionModel(selectionModel);
    ui->modTreeView->setSelectionModel(selectionModel);
    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &LocalModBrowser::updateSelectedMods);
    connect(this, &LocalModBrowser::selectedModsChanged, this, &LocalModBrowser::onSelectedModsChanged);
    connect(ui->modListView->verticalScrollBar(), &QScrollBar::valueChanged, this, &LocalModBrowser::updateListViewIndexWidget);
    connect(ui->modTreeView->verticalScrollBar(), &QScrollBar::valueChanged, this, &LocalModBrowser::updateTreeViewIndexWidget);
}

LocalModBrowser::~LocalModBrowser()
{
    delete ui;
}

void LocalModBrowser::load()
{
    if(!modPath_->modsLoaded())
        modPath_->loadMods();
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

void LocalModBrowser::updateUi()
{
    for(auto &&widget : findChildren<LocalModItemWidget *>())
        widget->updateUi();
}

void LocalModBrowser::onLoadStarted()
{
    ui->actionReload_Mods->setEnabled(false);
    ui->actionLink_Mod_Files->setEnabled(false);
    ui->actionCheck_Updates->setEnabled(false);
    ui->menuExport->setEnabled(false);
    ui->actionDelete_Old_Files_In_Path->setEnabled(false);
    ui->actionBatch_Rename->setEnabled(false);
    ui->actionUpdate_All->setEnabled(false);
    onLoadProgress(0, 0);
    statusBarWidget_->label()->setText(tr("Loading mod files..."));
}

void LocalModBrowser::onLoadProgress(int loadedCount, int totalCount)
{
    statusBarWidget_->label()->setText(tr("Loading mod files.. (Loaded %1/%2 mod files)").arg(loadedCount).arg(totalCount));
    progressBar_->setMaximum(totalCount);
    progressBar_->setValue(loadedCount);
}

void LocalModBrowser::onLoadFinished()
{
    ui->actionReload_Mods->setEnabled(true);
    if(!modPath_->isLinking()){
        ui->actionLink_Mod_Files->setEnabled(true);
        ui->actionCheck_Updates->setEnabled(true);
//        ui->actionUpdate_All->setEnabled(true);
    }
    ui->menuExport->setEnabled(true);
    ui->actionDelete_Old_Files_In_Path->setEnabled(true);
    ui->actionBatch_Rename->setEnabled(true);
    updateStatusText();
    ui->actionCheck_Updates->setText(tr("Check Updates"));
}

void LocalModBrowser::onLinkStarted()
{
    ui->actionLink_Mod_Files->setEnabled(false);
    ui->actionCheck_Updates->setEnabled(false);
    ui->actionUpdate_All->setEnabled(false);
    onLinkProgress(0, 0);
    statusBarWidget_->label()->setText(tr("Linking mod files..."));
}

void LocalModBrowser::onLinkProgress(int linkedCount, int totalCount)
{
    statusBarWidget_->label()->setText(tr("Linkinig mod files.. (Linked %1/%2 mod files)").arg(linkedCount).arg(totalCount));
    progressBar_->setMaximum(totalCount);
    progressBar_->setValue(linkedCount);
}

void LocalModBrowser::onLinkFinished()
{
    ui->actionLink_Mod_Files->setEnabled(true);
    if(!modPath_->isChecking()){
        ui->actionCheck_Updates->setEnabled(true);
        ui->actionUpdate_All->setEnabled(true);
    }
    updateStatusText();
}

void LocalModBrowser::onCheckUpdatesStarted()
{
    ui->actionUpdate_All->setEnabled(false);
    ui->actionCheck_Updates->setText(tr("Cancel Checking"));
    ui->updateWidget->setVisible(false);
    onUpdateCheckedCountUpdated(0, 0);
    progressBar_->setMaximum(modPath_->modList().count());
    statusBarWidget_->label()->setText(tr("Checking mod updates..."));
}

void LocalModBrowser::onCheckCancelled()
{
//    onCheckUpdatesFinished();
}

void LocalModBrowser::onUpdateCheckedCountUpdated(int checkedCount, int totalCount)
{
    if(!modPath_->isChecking()) return;
    statusBarWidget_->label()->setText(tr("%1 mods need update... (Checked %2/%3 mods)").arg(modPath_->updatableCount()).arg(checkedCount).arg(totalCount));
    progressBar_->setValue(checkedCount);
}

void LocalModBrowser::onCheckUpdatesFinished(bool success)
{
    if(modPath_->isUpdating()) return;
    ui->actionCheck_Updates->setText(tr("Check Updates"));
    onUpdatableCountChanged();
    updateStatusText();
    qDebug() << success << modPath_->updateChecker()->failedCount();
    if(!success) {
        auto &&failedObjects = modPath_->updateChecker()->failedObjects();
        QStringList stringlist;
        for(auto &&object : failedObjects){
            if(auto mod = qobject_cast<const LocalMod *>(object))
                stringlist << "<li>" + mod->displayName() + "</li>";
        }
        QMessageBox::information(this, tr("Update Checking Incompleted"), tr("%1 mods failed checking update because of network: <ul>%2</ul>")
                                 .arg(modPath_->updateChecker()->failedCount())
                                 .arg(stringlist.join("")));
    }
}

void LocalModBrowser::onUpdatableCountChanged()
{
    if(modPath_->isChecking()) return;
    if(auto count = modPath_->updatableCount()){
        ui->updateWidget->setVisible(true);
        ui->updateAllButton->setVisible(true);
        ui->actionUpdate_All->setEnabled(true);
        ui->actionUpdate_All->setText(tr("Update All"));
        ui->updateProgressText->setText(tr("%1 mods need update.").arg(count));
    } else {
        ui->updateWidget->setVisible(false);
        ui->actionUpdate_All->setEnabled(false);
        ui->actionUpdate_All->setText(tr("Nothing to Update"));
    }
}

void LocalModBrowser::onUpdatesStarted()
{
    ui->actionUpdate_All->setEnabled(false);
    onUpdatesProgress(0, 0);
    ui->actionUpdate_All->setText(tr("Updating..."));
}

void LocalModBrowser::onUpdatesProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    progressBar_->setValue(bytesReceived);
    progressBar_->setMaximum(bytesTotal);
}

void LocalModBrowser::onUpdatesDoneCountUpdated(int doneCount, int totalCount)
{
    statusBarWidget_->label()->setText(tr("Updating... (Updated %1/%2 mods)").arg(doneCount).arg(totalCount));
}

void LocalModBrowser::onUpdatesDone(int successCount, int failCount)
{
    auto str = tr("%1 mods in %2 has been updated. Enjoy it!").arg(successCount).arg(modPath_->info().displayName());
    if(failCount)
        str.append("\n").append(tr("Sadly, %1 mods failed to update.").arg(failCount));
    if(Config().getPostUpdate() == Config::Keep)
        str.append("\n").append(tr("You can revert update if find any incompatibility."));
    QMessageBox::information(this, tr("Update Finished"), str);

    onCheckUpdatesFinished();
}

void LocalModBrowser::updateStatusText()
{
    qDebug() << proxyModel_->rowCount() << model_->rowCount();
    statusBarWidget_->setModCount(proxyModel_->rowCount(), model_->rowCount());
}

void LocalModBrowser::updateProgressBar()
{
    bool bl = modPath_->isLoading() || modPath_->isLinking() || modPath_->isChecking() || modPath_->isUpdating();
    progressBar_->setVisible(bl);
    statusBarWidget_->label()->setVisible(bl);
}

QMenu *LocalModBrowser::getMenu(QList<LocalMod *> mods)
{
    auto menu = new QMenu(this);
    for(auto &&action : ui->menuMod->actions())
        if(action->isEnabled()) menu->addAction(action);
    return menu;
}

void LocalModBrowser::updateListViewIndexWidget()
{
    auto beginRow = ui->modListView->indexAt(QPoint(0, 0)).row();
    if(beginRow < 0) return;
    auto endRow = ui->modListView->indexAt(QPoint(0, ui->modListView->height())).row();
    if(endRow < 0)
        endRow = proxyModel_->rowCount() - 1;
    else
        //extra 2
        endRow += 2;
    for(int row = beginRow; row <= endRow && row < proxyModel_->rowCount(); row++){
        auto index = proxyModel_->index(row, 0);
        if(ui->modListView->indexWidget(index)) continue;
//        qDebug() << "new widget at row" << row;
        auto mod = proxyModel_->data(index, Qt::UserRole + 1).value<LocalMod*>();
        if(mod){
            auto modItemWidget = new LocalModItemWidget(ui->modListView, mod);
            ui->modListView->setIndexWidget(index, modItemWidget);
            model_->setItemHeight(modItemWidget->height());
        }
    }
}

void LocalModBrowser::updateTreeViewIndexWidget()
{
    auto beginRow = ui->modTreeView->indexAt(QPoint(0, 0)).row();
    if(beginRow < 0) return;
    auto endRow = ui->modTreeView->indexAt(QPoint(0, ui->modTreeView->height())).row();
    if(endRow < 0)
        endRow = proxyModel_->rowCount() - 1;
    else
        //extra 2
        endRow += 2;
    for(int row = beginRow; row <= endRow && row < proxyModel_->rowCount(); row++){
        if(ui->modTreeView->indexWidget(proxyModel_->index(row, LocalModPathModel::TagsColumn))) continue;
//        qDebug() << "new widget at row" << row;
        auto mod = proxyModel_->data(proxyModel_->index(row, LocalModPathModel::ModColumn), Qt::UserRole + 1).value<LocalMod*>();
        if(mod){
            auto tagsWidget = new TagsWidget(this);
            tagsWidget->setMod(mod);
            ui->modTreeView->setIndexWidget(proxyModel_->index(row, LocalModPathModel::TagsColumn), tagsWidget);
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
    return ui->menuMod->actions();
}

QList<QAction *> LocalModBrowser::pathActions() const
{
    return ui->menuPath->actions();
}

QIcon LocalModBrowser::icon() const
{
    return modPath_->info().icon();
}

QString LocalModBrowser::name() const
{
    return modPath_->info().displayName();
}

LocalModPath *LocalModBrowser::modPath() const
{
    return modPath_;
}

void LocalModBrowser::onItemDoubleClicked(const QModelIndex &index)
{
    auto mod = proxyModel_->data(index.siblingAtColumn(LocalModPathModel::ModColumn), Qt::UserRole + 1).value<LocalMod*>();
    if(!mod) return;
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
    for(int column = LocalModPathModel::ModColumn + 1; column < model_->columnCount(); column++){
        if(column == LocalModPathModel::NameColumn) continue;
        auto text = model_->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();
        auto icon = model_->headerData(column, Qt::Horizontal, Qt::DecorationRole).value<QIcon>();
        auto action = menu->addAction(icon, text);
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
    updateListViewIndexWidget();
    updateTreeViewIndexWidget();
    QWidget::paintEvent(event);
}

void LocalModBrowser::setPageSwitcher(PageSwitcher *newPageSwitcher)
{
    pageSwitcher_ = newPageSwitcher;
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
        list << proxyModel_->data(index, Qt::UserRole + 1).value<LocalMod*>();
    return list;
}

void LocalModBrowser::on_actionOpen_Folder_triggered()
{
    openFolder(modPath_->info().path());
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
    ui->actionMove_Selected_Mods_to->setEnabled(!noMod);
    ui->actionCheck_Updates_for_Selected_Mods->setEnabled(!noMod);

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

    if(selectedMods_.count() == 1){
        ui->actionShow_This_Mod_in_Directory->setEnabled(true);
        ui->actionOpen_Mod_Dialog->setEnabled(true);
        ui->actionSearch_on_Curseforge->setEnabled(true);
        ui->actionSearch_on_Modrinth->setEnabled(true);
        auto mod = selectedMods_.first();
        auto url = mod->commonInfo()->sources();
        ui->actionOpen_In_GitHub->setEnabled(url.host() == "github.com" && url.path().count('/') == 2);
        ui->actionOpen_Curseforge_Mod_Dialog->setEnabled(mod->curseforgeMod());
        ui->actionOpen_Modrinth_Mod_Dialog->setEnabled(mod->modrinthMod());
    } else{
        ui->actionShow_This_Mod_in_Directory->setEnabled(false);
        ui->actionOpen_Mod_Dialog->setEnabled(false);
        ui->actionSearch_on_Curseforge->setEnabled(false);
        ui->actionSearch_on_Modrinth->setEnabled(false);
        ui->actionOpen_In_GitHub->setEnabled(false);
        ui->actionOpen_Curseforge_Mod_Dialog->setEnabled(false);
        ui->actionOpen_Modrinth_Mod_Dialog->setEnabled(false);
    }
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
        browser->load();
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
        browser->load();
        dialog->exec();
    }
}

void LocalModBrowser::on_actionCheck_Updates_triggered()
{
    if(!modPath_->isChecking())
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

void LocalModBrowser::on_actionExport_manifest_json_triggered()
{
    auto dialog = new ExportManifestJsonDialog(this, modPath_, ExportManifestJsonDialog::MANIFEST_JSON);
    dialog->exec();
}

void LocalModBrowser::on_actionExport_modlist_html_Cuseforge_triggered()
{
    auto dialog = new ExportManifestJsonDialog(this, modPath_, ExportManifestJsonDialog::CF_HTML);
    dialog->exec();
}

void LocalModBrowser::on_actionExport_modlist_html_Modrinth_triggered()
{
    auto dialog = new ExportManifestJsonDialog(this, modPath_, ExportManifestJsonDialog::M_HTML);
    dialog->exec();
}


void LocalModBrowser::on_actionOpen_In_GitHub_triggered()
{
    if(selectedMods_.isEmpty()) return;
    auto mod = selectedMods_.first();
    auto url = mod->commonInfo()->sources();
    if(url.host() != "github.com") return;
    auto list = url.path().split("/");
    if(list.size() < 3) return;
    qDebug() << list.at(1) << list.at(2);
    GitHubRepoInfo repoInfo(list.at(1), list.at(2));
    auto browser = new GitHubRepoBrowser(this, repoInfo);
    browser->show();
}

void LocalModBrowser::on_actionExport_Compressed_File_triggered()
{
    //TODO: export configure dialog
    auto filePath = QFileDialog::getSaveFileName(this, tr("Save your compressed file"),
                                                  QDir(modPath_->info().path()).filePath("mods.zip"), "*.zip");
    if(filePath.isEmpty()) return;
    QStringList list;
    for(int i = 0; i < model_->rowCount(); i++){
        auto mod = model_->index(i, LocalModPathModel::ModColumn).data(Qt::UserRole + 1).value<LocalMod*>();
        list << mod->modFile()->fileInfo().absoluteFilePath();
    }
    list << modPath_->modsJsonFilePath();
    QtConcurrent::run([=]{
        if(!JlCompress::compressFiles(filePath, list))
            QMessageBox::warning(this, tr("ZIP Failed"), tr("Failed to create zip file"));
        else
            openFileInFolder(filePath);
    });
}

void LocalModBrowser::on_actionShow_This_Mod_in_Directory_triggered()
{
    if(selectedMods_.isEmpty()) return;
    auto mod = selectedMods_.first();
    openFileInFolder(mod->modFile()->path());
}

void LocalModBrowser::on_actionCheck_Updates_for_Selected_Mods_triggered()
{
    if(selectedMods_.isEmpty()) return;
    for(auto &&mod : selectedMods_)
        mod->checkUpdates();
}

