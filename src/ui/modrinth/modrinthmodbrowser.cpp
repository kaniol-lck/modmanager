#include "modrinthmodbrowser.h"
#include "ui_modrinthmodbrowser.h"

#include <QScrollBar>
#include <QDir>
#include <QMenu>
#include <QClipboard>
#include <QStandardItem>
#include <QDesktopServices>

#include "modrinthmodinfowidget.h"
#include "modrinthfilelistwidget.h"
#include "ui/explorestatusbarwidget.h"
#include "modrinthmoditemwidget.h"
#include "modrinthmoddialog.h"
#include "local/localmodpathmanager.h"
#include "local/localmodpath.h"
#include "modrinth/modrinthmod.h"
#include "modrinth/modrinthapi.h"
#include "gameversion.h"
#include "modloadertype.h"
#include "config.hpp"
#include "util/funcutil.h"
#include "util/smoothscrollbar.h"
#include "util/unclosedmenu.h"
#include "local/localmod.h"

ModrinthModBrowser::ModrinthModBrowser(QWidget *parent, LocalMod *localMod) :
    ExploreBrowser(parent, QIcon(":/image/modrinth.svg"), "Modrinth", QUrl("https://modrinth.com/mods")),
    ui(new Ui::ModrinthModBrowser),
    infoWidget_(new ModrinthModInfoWidget(this)),
    fileListWidget_(new ModrinthFileListWidget(this)),
    api_(new ModrinthAPI(this)),
    localMod_(localMod)
{
    infoWidget_->hide();
    fileListWidget_->hide();
    ui->setupUi(this);
    initUi();

    //setup status bar
    ui->statusbar->addPermanentWidget(statusBarWidget_);

    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->label);
    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->versionSelectButton);
    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->label_5);
    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->categorySelectButton);
    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->label_3);
    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->loaderSelect);
    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->label_4);
    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->downloadPathSelect);

    ui->searchBar->addWidget(ui->searchText);
    ui->searchBar->addWidget(ui->sortSelect);

    ui->loaderSelect->blockSignals(true);
    for(const auto &type : ModLoaderType::modrinth)
        ui->loaderSelect->addItem(ModLoaderType::icon(type), ModLoaderType::toString(type));
    ui->loaderSelect->blockSignals(false);

    updateVersionList();
    updateCategoryList();
    updateLocalPathList();
    updateStatusText();

    connect(ui->searchText, &QLineEdit::returnPressed, this, &ModrinthModBrowser::search);
    connect(VersionManager::manager(), &VersionManager::modrinthVersionListUpdated, this, &ModrinthModBrowser::updateVersionList);
    connect(LocalModPathManager::manager(), &LocalModPathManager::pathListUpdated, this, &ModrinthModBrowser::updateLocalPathList);
    connect(this, &ModrinthModBrowser::downloadPathChanged, fileListWidget_, &ModrinthFileListWidget::setDownloadPath);

    if(localMod_){
        currentName_ = localMod_->commonInfo()->id();
        ui->searchText->setText(currentName_);
    } else if(Config().getSearchModsOnStartup()){
        inited_ = true;
        getModList(currentName_);
    }
}

ModrinthModBrowser::~ModrinthModBrowser()
{
    delete ui;
    for(auto row = 0; row < model_->rowCount(); row++){
        auto item = model_->item(row);
        auto mod = item->data().value<ModrinthMod*>();
        if(mod && !mod->parent())
            mod->deleteLater();
    }
}

void ModrinthModBrowser::load()
{
    if(!inited_){
        inited_ = true;
        getModList(currentName_);
    }
}

QWidget *ModrinthModBrowser::infoWidget() const
{
    return infoWidget_;
}

QWidget *ModrinthModBrowser::fileListWidget() const
{
    return fileListWidget_;
}

void ModrinthModBrowser::refresh()
{
    search();
}

void ModrinthModBrowser::searchModByPathInfo(const LocalModPathInfo &info)
{
    currentGameVersions_ = { info.gameVersion() };
    ui->versionSelectButton->setText(info.gameVersion());
    ui->loaderSelect->blockSignals(true);
    ui->loaderSelect->setCurrentIndex(ModLoaderType::modrinth.indexOf(info.loaderType()));
    ui->loaderSelect->blockSignals(false);
    ui->downloadPathSelect->setCurrentText(info.displayName());
    getModList(currentName_);
}

void ModrinthModBrowser::updateUi()
{
//    for(int i = 0; i < model_->rowCount(); i++){
//        if(auto widget = modListView_->indexWidget(model_->index(i, 0)))
//            dynamic_cast<ModrinthModItemWidget*>(widget)->updateUi();
//    }
}

void ModrinthModBrowser::updateVersionList()
{
    auto menu = new UnclosedMenu;
    menu->setStyleSheet("QMenu { menu-scrollable: 1; }");

    auto multiSelectionAction = menu->addAction(tr("Multi Selection"));
    multiSelectionAction->setCheckable(true);
    connect(multiSelectionAction, &QAction::toggled, menu, &UnclosedMenu::setUnclosed);
    connect(multiSelectionAction, &QAction::toggled, this, [=](bool checked){
        Config().setModrinthMultiVersion(checked);
        if(!checked && currentGameVersions_.size() > 1){
            auto version = currentGameVersions_.first();
            ui->versionSelectButton->setText(version);
            currentGameVersions_ = { version };
            lastGameVersions_ = currentGameVersions_;
            getModList(currentName_);
        }
    });
    auto showSnapshotAction = menu->addAction(tr("Show Snapshot"));
    showSnapshotAction->setCheckable(true);
    connect(showSnapshotAction, &QAction::toggled, this, [=](bool checked){
        Config().setShowModrinthSnapshot(checked);
        for(auto &&action : menu->actions()){
            if(action->data().toBool())
                action->setVisible(checked);
            if(auto &&submenu = action->menu()){
                for (auto &&subaction : submenu->actions()) {
                    if(subaction->data().toBool())
                        subaction->setVisible(checked);
                }
            }
        }
    });
    menu->addSeparator();
    auto anyVersionAction = menu->addAction(tr("Any"));
    connect(anyVersionAction, &QAction::triggered, this, [=]{
        currentGameVersions_.clear();
        ui->versionSelectButton->setText(tr("Any"));
        ui->versionSelectButton->setIcon(QIcon());
    });
    auto getVersionAction = [=](const GameVersion &version){
        auto versionAction = new QAction(version);
        versionAction->setCheckable(multiSelectionAction->isChecked());
        connect(versionAction, &QAction::triggered, this, [=](bool checked){
            if(multiSelectionAction->isChecked()){
                if(checked)
                    currentGameVersions_ << version;
                else
                    currentGameVersions_.removeAll(version);
            }else {
                currentGameVersions_.clear();
                currentGameVersions_ << version;
                //only search for single selection
                if(currentGameVersions_ == lastGameVersions_) return;
                lastGameVersions_ = currentGameVersions_;
                getModList(currentName_);
            }
            ui->versionSelectButton->setToolTip("");
            if(auto size = currentGameVersions_.size(); size == 0)
                ui->versionSelectButton->setText(tr("Any"));
            else if(size == 1)
                ui->versionSelectButton->setText(version);
            else{
                QStringList list;
                for(const auto &version : qAsConst(currentGameVersions_))
                    list << version;
                ui->versionSelectButton->setText(tr("%1 etc.").arg(list.first()));
                ui->versionSelectButton->setToolTip(list.join(tr(", ")));
            }
        });
        connect(multiSelectionAction, &QAction::triggered, versionAction, &QAction::setCheckable);
        connect(anyVersionAction, &QAction::triggered, versionAction, &QAction::setChecked);
        return versionAction;
    };

    anyVersionAction->trigger();
    menu->addSeparator();
    QMap<QString, QMenu*> submenus;
    QList<QString> keys; //to keep order
    for(auto &&version : GameVersion::modrinthVersionList()){
        QString submenuName;
        QString type = version.type();
        if(!type.isEmpty() && type != "release"){
            if(type == "snapshot" && version.majorVersion() == GameVersion::Any)
                submenuName = tr("Future Version");
            else
                submenuName = type;
        }
        if(version.majorVersion() != GameVersion::Any)
            submenuName = version.majorVersion();
        if(submenuName.isEmpty() || submenus.contains(submenuName)) continue;
        auto submenu = new UnclosedMenu(submenuName);
        if(type != "release")
            submenu->menuAction()->setData(true);
//        submenu->setUnclosed(multiSelectionAction->isChecked());
        connect(multiSelectionAction, &QAction::toggled, submenu, &UnclosedMenu::setUnclosed);
        submenus[submenuName] = submenu;
        keys << submenuName;
    }
    for(auto &&version : GameVersion::modrinthVersionList()){
        QString submenuName;
        QString type = version.type();
        if(!type.isEmpty() && type != "release"){
            if(type == "snapshot" && version.majorVersion() == GameVersion::Any)
                submenuName = tr("Future Version");
            else
                submenuName = type;
        }
        if(version.majorVersion() != GameVersion::Any)
            submenuName = version.majorVersion();
        if(submenus.contains(submenuName)){
            auto submenu = submenus[submenuName];
            if(submenu->actions().size() == 1)
                if(GameVersion version = submenu->actions().at(0)->text(); version.id() == submenuName)
                    submenu->addSeparator();
            auto action = getVersionAction(version);
            if(version.type() != "release")
                action->setData(true);
            submenu->addAction(action);
        }
    }
    for(const auto &key : qAsConst(keys)){
        auto submenu = submenus[key];
        if(submenu->actions().size() == 1)
            menu->addActions(submenu->actions());
        else
            menu->addMenu(submenu);
    }
    multiSelectionAction->setChecked(Config().getModrinthMultiVersion());
    multiSelectionAction->trigger();
    showSnapshotAction->setChecked(Config().getShowModrinthSnapshot());
    showSnapshotAction->trigger();
    connect(menu, &QMenu::aboutToHide, this, [=]{
        //only search for multi selection
        if(!multiSelectionAction->isChecked()) return;
        if(currentGameVersions_ == lastGameVersions_) return;
        lastGameVersions_ = currentGameVersions_;
        getModList(currentName_);
    });
    ui->versionSelectButton->setMenu(menu);
}

void ModrinthModBrowser::updateCategoryList()
{
    auto menu = new UnclosedMenu;
    menu->setStyleSheet("QMenu { menu-scrollable: 1; }");

    auto multiSelectionAction = menu->addAction(tr("Multi Selection"));
    multiSelectionAction->setCheckable(true);
    multiSelectionAction->setChecked(Config().getModrinthMultiCategory());
    menu->setUnclosed(multiSelectionAction->isChecked());
    connect(multiSelectionAction, &QAction::toggled, menu, &UnclosedMenu::setUnclosed);
    connect(multiSelectionAction, &QAction::toggled, this, [=](bool checked){
        Config().setModrinthMultiCategory(checked);
        if(!checked && currentCategoryIds_.size() > 1){
            auto categoryId = currentCategoryIds_.first();
            auto &&categories = ModrinthAPI::getCategories();
            if(auto it = std::find_if(categories.cbegin(), categories.cend(), [=](const auto &val){
                return std::get<1>(val) == categoryId;
            }); it != categories.cend()){
                auto &&[name, id] = *it;
                ui->categorySelectButton->setText(name);
                QIcon icon(QString(":/image/modrinth/%1.svg").arg(id));
                ui->categorySelectButton->setIcon(icon);
            }
            currentCategoryIds_.clear();
            currentCategoryIds_ << categoryId;
            lastCategoryIds_ = currentCategoryIds_;
            getModList(currentName_);
        }
    });
    menu->addSeparator();
    auto anyCategoryAction = menu->addAction(tr("Any"));
    connect(anyCategoryAction, &QAction::triggered, this, [=]{
        currentCategoryIds_.clear();
        ui->categorySelectButton->setText(tr("Any"));
        ui->categorySelectButton->setIcon(QIcon());
    });
    anyCategoryAction->trigger();
    menu->addSeparator();
    for(auto &&[name, id] : ModrinthAPI::getCategories()){
        QIcon icon(QString(":/image/modrinth/%1.svg").arg(id));
        auto categoryAction = menu->addAction(icon, name);
        categoryAction->setCheckable(multiSelectionAction->isChecked());
        connect(categoryAction, &QAction::triggered, this, [=, id = id, name = name](bool checked){
            if(multiSelectionAction->isChecked()){
                if(checked)
                    currentCategoryIds_ << id;
                else
                    currentCategoryIds_.removeAll(id);
            }else {
                currentCategoryIds_.clear();
                currentCategoryIds_ << id;
                //only search for single selection
                if(currentCategoryIds_ == lastCategoryIds_) return;
                lastCategoryIds_ = currentCategoryIds_;
                getModList(currentName_);
            }
            ui->categorySelectButton->setIcon(QIcon());
            ui->categorySelectButton->setToolTip("");
            if(auto size = currentCategoryIds_.size(); size == 0)
                ui->categorySelectButton->setText(tr("Any"));
            else if(size == 1){
                ui->categorySelectButton->setText(name);
                ui->categorySelectButton->setIcon(icon);
            } else{
                QStringList list;
                for(const auto &categoryId: qAsConst(currentCategoryIds_)){
                    auto &&categories = ModrinthAPI::getCategories();
                    if(auto it = std::find_if(categories.cbegin(), categories.cend(), [=](const auto &val){
                        return std::get<1>(val) == categoryId;
                    }); it != categories.cend())
                        list << std::get<0>(*it);
                }
                ui->categorySelectButton->setText(tr("%1 etc.").arg(list.first()));
                ui->categorySelectButton->setToolTip(list.join(tr(", ")));
            }
        });
        connect(multiSelectionAction, &QAction::triggered, categoryAction, &QAction::setCheckable);
        connect(anyCategoryAction, &QAction::triggered, categoryAction, &QAction::setChecked);
    }
    connect(menu, &QMenu::aboutToHide, this, [=]{
        //only search for multi selection
        if(!multiSelectionAction->isChecked()) return;
        if(currentCategoryIds_ == lastCategoryIds_) return;
        lastCategoryIds_ = currentCategoryIds_;
        getModList(currentName_);
    });
    ui->categorySelectButton->setMenu(menu);
}

void ModrinthModBrowser::updateLocalPathList()
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

void ModrinthModBrowser::search()
{
//    if(ui->searchText->text() == currentName_) return;
    currentName_ = ui->searchText->text();
    getModList(currentName_);
}

void ModrinthModBrowser::updateStatusText()
{
    auto str = tr("Loaded %1 mods from Modrinth.").arg(model_->rowCount());
    ui->statusbar->showMessage(str);
}

void ModrinthModBrowser::getModList(QString name, int index)
{
    if(!index)
        currentIndex_ = 0;
    else if(!hasMore_ || isSearching_)
        return;
    setCursor(Qt::BusyCursor);
    statusBarWidget_->setText(tr("Searching mods..."));
    statusBarWidget_->setProgressVisible(true);

    auto sort = ui->sortSelect->currentIndex();
    auto type = ModLoaderType::modrinth.at(ui->loaderSelect->currentIndex());

    isSearching_ = true;
    auto conn = api_->searchMods(name, currentIndex_, currentGameVersions_, type, currentCategoryIds_, sort, [=](const QList<ModrinthModInfo> &infoList){
        setCursor(Qt::ArrowCursor);
        statusBarWidget_->setText("");
        statusBarWidget_->setProgressVisible(false);

        //new search
        if(currentIndex_ == 0){
            for(auto row = 0; row < model_->rowCount(); row++){
                auto item = model_->item(row);
                auto mod = item->data().value<ModrinthMod*>();
                if(mod && !mod->parent())
                    mod->deleteLater();
            }
            model_->clear();
            hasMore_ = true;
        }

        //show them
        for(const auto &info : qAsConst(infoList)){
            auto mod = new ModrinthMod((QObject*)nullptr, info);
            auto *item = new QStandardItem();
            item->setData(QVariant::fromValue(mod));
            item->setSizeHint(QSize(0, 100));
            model_->appendRow(item);
            mod->acquireIcon();
        }
        if(infoList.size() < Config().getSearchResultCount()){
            auto item = new QStandardItem(tr("There is no more mod here..."));
            item->setSizeHint(QSize(0, 108));
            auto font = qApp->font();
            font.setPointSize(20);
            item->setFont(font);
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            model_->appendRow(item);
            hasMore_ = false;
        }
        isSearching_ = false;
        updateStatusText();
    });
    connect(this, &QObject::destroyed, this, [=]{
        disconnect(conn);
    });
}

QDialog *ModrinthModBrowser::getDialog(QStandardItem *item)
{
    auto mod = item->data().value<ModrinthMod*>();
    if(mod && !mod->parent()){
        auto dialog = new ModrinthModDialog(this, mod);
        //set parent
        mod->setParent(dialog);
        dialog->setDownloadPath(downloadPath_);
        connect(this, &ModrinthModBrowser::downloadPathChanged, dialog, &ModrinthModDialog::setDownloadPath);
        connect(dialog, &ModrinthModDialog::finished, this, [=]{
            mod->setParent(nullptr);
        });
        return dialog;
    }
    return nullptr;
}

void ModrinthModBrowser::on_sortSelect_currentIndexChanged(int)
{
    getModList(currentName_);
}

void ModrinthModBrowser::on_loaderSelect_currentIndexChanged(int)
{
    getModList(currentName_);
}

void ModrinthModBrowser::on_downloadPathSelect_currentIndexChanged(int index)
{
    if(index < 0 || index >= ui->downloadPathSelect->count()) return;
    if(index == 0)
        downloadPath_ = nullptr;
    else
        downloadPath_ =  LocalModPathManager::pathList().at(index - 1);
    emit downloadPathChanged(downloadPath_);
}

void ModrinthModBrowser::onSelectedItemChanged(QStandardItem *item)
{
    if(item)
        selectedMod_ = item->data().value<ModrinthMod*>();
    else
        selectedMod_ = nullptr;
    ui->actionOpen_Modrinth_Mod_Dialog->setEnabled(selectedMod_);
    ui->menuDownload->setEnabled(selectedMod_);
    ui->actionCopy_Website_Link->setEnabled(selectedMod_);
    ui->actionOpen_Website_Link->setEnabled(selectedMod_);
    emit selectedModsChanged(selectedMod_);
    infoWidget_->setMod(selectedMod_);
    fileListWidget_->setMod(selectedMod_);
}

QWidget *ModrinthModBrowser::getIndexWidget(QStandardItem *item)
{
    auto mod = item->data().value<ModrinthMod*>();
    if(mod){
        auto widget = new ModrinthModItemWidget(nullptr, mod);
        widget->setDownloadPath(downloadPath_);
        connect(this, &ModrinthModBrowser::downloadPathChanged, widget, &ModrinthModItemWidget::setDownloadPath);
        return widget;
    } else
        return nullptr;
}

void ModrinthModBrowser::loadMore()
{
    if(!isSearching_ && hasMore_){
        currentIndex_ += Config().getSearchResultCount();
        getModList(currentName_, currentIndex_);
    }
}

ModrinthMod *ModrinthModBrowser::selectedMod() const
{
    return selectedMod_;
}

QList<QAction *> ModrinthModBrowser::modActions() const
{
    return ui->menu_Mod->actions();
}

ExploreBrowser *ModrinthModBrowser::another()
{
    return new ModrinthModBrowser;
}

void ModrinthModBrowser::on_actionOpen_Folder_triggered()
{
    QString path;
    if(downloadPath_)
        path = downloadPath_->info().path();
    else
        path = Config().getDownloadPath();
    openFileInFolder(path);
}

void ModrinthModBrowser::on_menuDownload_aboutToShow()
{
    ui->menuDownload->clear();
    ui->menuDownload->setStyleSheet("QMenu { menu-scrollable: 1; }");
    if(!selectedMod_) return;
    for(auto &&file : selectedMod_->modInfo().fileList()){
        ui->menuDownload->addAction(file.displayName(), this, [=]{
            selectedMod_->download(file, downloadPath_);
        });
    }
}

void ModrinthModBrowser::on_actionCopy_Website_Link_triggered()
{
    if(!selectedMod_) return;
    QApplication::clipboard()->setText(selectedMod_->modInfo().websiteUrl().toString());
}

void ModrinthModBrowser::on_actionOpen_Modrinth_Mod_Dialog_triggered()
{
    if(!selectedMod_) return;
    if(selectedMod_ && !selectedMod_->parent()){
        auto dialog = new ModrinthModDialog(this, selectedMod_);
        //set parent
        selectedMod_->setParent(dialog);
        dialog->setDownloadPath(downloadPath_);
        connect(this, &ModrinthModBrowser::downloadPathChanged, dialog, &ModrinthModDialog::setDownloadPath);
        connect(dialog, &ModrinthModDialog::finished, this, [=]{
            selectedMod_->setParent(nullptr);
        });
        dialog->show();
    }
}

QMenu *ModrinthModBrowser::getMenu()
{
    auto menu = new QMenu(this);
    menu->addAction(ui->actionOpen_Modrinth_Mod_Dialog);
    menu->addMenu(ui->menuDownload);
    menu->addAction(ui->actionCopy_Website_Link);
    menu->addAction(ui->actionOpen_Website_Link);
    return menu;
}

void ModrinthModBrowser::on_actionOpen_Website_Link_triggered()
{
    if(!selectedMod_) return;
    QDesktopServices::openUrl(selectedMod_->modInfo().websiteUrl());
}
