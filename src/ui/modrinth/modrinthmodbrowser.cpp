#include "modrinthmodbrowser.h"
#include "ui_modrinthmodbrowser.h"

#include <QScrollBar>
#include <QDir>
#include <QMenu>

#include "modrinthmodinfowidget.h"
#include "modrinthfilelistwidget.h"
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

ModrinthModBrowser::ModrinthModBrowser(QWidget *parent) :
    ExploreBrowser(parent, QIcon(":/image/modrinth.svg"), "Modrinth", QUrl("https://modrinth.com/mods")),
    ui(new Ui::ModrinthModBrowser),
    infoWidget_(new ModrinthModInfoWidget(this)),
    fileListWidget_(new ModrinthFileListWidget(this)),
    api_(new ModrinthAPI(this))
{
    ui->setupUi(this);
    ui->modListWidget->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->modListWidget->setProperty("class", "ModList");

    for(const auto &type : ModLoaderType::modrinth)
        ui->loaderSelect->addItem(ModLoaderType::icon(type), ModLoaderType::toString(type));

    connect(ui->modListWidget->verticalScrollBar(), &QAbstractSlider::valueChanged,  this , &ModrinthModBrowser::onSliderChanged);
    connect(ui->searchText, &QLineEdit::returnPressed, this, &ModrinthModBrowser::search);

    updateVersionList();
    connect(VersionManager::manager(), &VersionManager::modrinthVersionListUpdated, this, &ModrinthModBrowser::updateVersionList);

    updateCategoryList();

    updateLocalPathList();
    connect(LocalModPathManager::manager(), &LocalModPathManager::pathListUpdated, this, &ModrinthModBrowser::updateLocalPathList);

    getModList(currentName_);
    isUiSet_ = true;
}

ModrinthModBrowser::~ModrinthModBrowser()
{
    delete ui;
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
    isUiSet_ = false;
    currentGameVersions_ = { info.gameVersion() };
    ui->versionSelectButton->setText(info.gameVersion());
    ui->loaderSelect->setCurrentIndex(ModLoaderType::modrinth.indexOf(info.loaderType()));
    isUiSet_ = true;
    ui->downloadPathSelect->setCurrentText(info.displayName());
    getModList(currentName_);
}

void ModrinthModBrowser::updateUi()
{
    updateVersionList();
    for(int i = 0; i < ui->modListWidget->count(); i++){
        auto item = ui->modListWidget->item(i);
        if(!item->text().isEmpty()) continue;
        auto widget = ui->modListWidget->itemWidget(item);
        dynamic_cast<ModrinthModItemWidget*>(widget)->updateUi();
    }
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
        connect(versionAction, &QAction::toggled, this, [=](bool checked){
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
                ui->versionSelectButton->setText(tr("%1 etc.").arg(version));
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
        submenu->setUnclosed(multiSelectionAction->isChecked());
        connect(multiSelectionAction, &QAction::triggered, submenu, &UnclosedMenu::setUnclosed);
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
                ui->categorySelectButton->setText(tr("%1 etc.").arg(name));
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

void ModrinthModBrowser::onSliderChanged(int i)
{
    if(!isSearching_ && hasMore_ && i >= ui->modListWidget->verticalScrollBar()->maximum() - 1000){
        currentIndex_ += Config().getSearchResultCount();
        getModList(currentName_, currentIndex_);
    }
}

void ModrinthModBrowser::getModList(QString name, int index)
{
    if(!index)
        currentIndex_ = 0;
    else if(!hasMore_ || isSearching_)
        return;
    setCursor(Qt::BusyCursor);

    auto sort = ui->sortSelect->currentIndex();
    auto type = ModLoaderType::modrinth.at(ui->loaderSelect->currentIndex());

    isSearching_ = true;
    auto conn = api_->searchMods(name, currentIndex_, currentGameVersions_, type, currentCategoryIds_, sort, [=](const QList<ModrinthModInfo> &infoList){
        setCursor(Qt::ArrowCursor);

        //new search
        if(currentIndex_ == 0){
            for(int i = 0; i < ui->modListWidget->count(); i++)
                ui->modListWidget->itemWidget(ui->modListWidget->item(i))->deleteLater();
            ui->modListWidget->clear();
            hasMore_ = true;
        }

        //show them
        for(const auto &info : qAsConst(infoList)){
            auto mod = new ModrinthMod(this, info);

            auto *listItem = new QListWidgetItem();
            auto modItemWidget = new ModrinthModItemWidget(ui->modListWidget, mod);
            listItem->setSizeHint(QSize(0, modItemWidget->height()));
            mod->setParent(modItemWidget);
            modItemWidget->setDownloadPath(downloadPath_);
            connect(this, &ModrinthModBrowser::downloadPathChanged, modItemWidget, &ModrinthModItemWidget::setDownloadPath);
            ui->modListWidget->addItem(listItem);
            ui->modListWidget->setItemWidget(listItem, modItemWidget);
            mod->acquireIcon();
        }
        if(infoList.size() < Config().getSearchResultCount()){
            auto item = new QListWidgetItem(tr("There is no more mod here..."));
            item->setSizeHint(QSize(0, 108));
            auto font = qApp->font();
            font.setPointSize(20);
            item->setFont(font);
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->modListWidget->addItem(item);
            hasMore_ = false;
        }
        isSearching_ = false;
    });
    connect(this, &QObject::destroyed, this, [=]{
        disconnect(conn);
    });
}

void ModrinthModBrowser::on_modListWidget_doubleClicked(const QModelIndex &index)
{
    auto item = ui->modListWidget->item(index.row());
    if(item->data(Qt::UserRole + 1).toBool()) return;
    item->setData(Qt::UserRole + 1, true);
    if(!item->text().isEmpty()) return;
    auto widget = dynamic_cast<ModrinthModItemWidget*>(ui->modListWidget->itemWidget(item));
    auto mod = widget->mod();
    auto dialog = new ModrinthModDialog(this, mod);
    //set parent
    mod->setParent(dialog);
    dialog->setDownloadPath(downloadPath_);
    connect(this, &ModrinthModBrowser::downloadPathChanged, dialog, &ModrinthModDialog::setDownloadPath);
    connect(dialog, &ModrinthModDialog::finished, widget, [=]{
        mod->setParent(widget);
        item->setData(Qt::UserRole + 1, false);
    });
    dialog->show();
}

void ModrinthModBrowser::on_sortSelect_currentIndexChanged(int)
{
    if(isUiSet_) getModList(currentName_);
}

void ModrinthModBrowser::on_versionSelect_currentIndexChanged(int)
{
    if(isUiSet_) getModList(currentName_);
}

void ModrinthModBrowser::on_loaderSelect_currentIndexChanged(int)
{
    if(isUiSet_) getModList(currentName_);
}

void ModrinthModBrowser::on_openFolderButton_clicked()
{
    QString path;
    if(downloadPath_)
        path = downloadPath_->info().path();
    else
        path = Config().getDownloadPath();
    openFileInFolder(path);
}

void ModrinthModBrowser::on_downloadPathSelect_currentIndexChanged(int index)
{
    if(!isUiSet_ || index < 0 || index >= ui->downloadPathSelect->count()) return;
    if(index == 0)
        downloadPath_ = nullptr;
    else
        downloadPath_ =  LocalModPathManager::pathList().at(index - 1);
    emit downloadPathChanged(downloadPath_);
}

void ModrinthModBrowser::on_categorySelect_currentIndexChanged(int)
{
    if(isUiSet_) getModList(currentName_);
}

void ModrinthModBrowser::on_modListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous[[maybe_unused]])
{
    if(!current || !current->text().isEmpty()) return;
    auto widget = ui->modListWidget->itemWidget(current);
    auto mod = dynamic_cast<ModrinthModItemWidget*>(widget)->mod();
    infoWidget_->setMod(mod);
    fileListWidget_->setMod(mod);
}
