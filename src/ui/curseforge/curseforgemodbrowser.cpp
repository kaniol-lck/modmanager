#include "curseforgemodbrowser.h"
#include "ui_curseforgemodbrowser.h"

#include <QScrollBar>
#include <QDir>
#include <QDebug>
#include <QAction>
#include <QMenu>

#include "curseforgemodinfowidget.h"
#include "curseforgefilelistwidget.h"
#include "local/localmodpathmanager.h"
#include "local/localmodpath.h"
#include "curseforge/curseforgemod.h"
#include "curseforge/curseforgeapi.h"
#include "curseforgemoditemwidget.h"
#include "curseforgemoddialog.h"
#include "gameversion.h"
#include "modloadertype.h"
#include "config.hpp"
#include "util/funcutil.h"
#include "util/smoothscrollbar.h"

CurseforgeModBrowser::CurseforgeModBrowser(QWidget *parent) :
    ExploreBrowser(parent, QIcon(":/image/curseforge.svg"), "Curseforge", QUrl("https://www.curseforge.com/minecraft/mc-mods")),
    ui(new Ui::CurseforgeModBrowser),
    infoWidget_(new CurseforgeModInfoWidget(this)),
    fileListWidget_(new CurseforgeFileListWidget(this)),
    api_(new CurseforgeAPI(this))
{
    ui->setupUi(this);
    ui->modListWidget->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->modListWidget->setProperty("class", "ModList");

    for(const auto &type : ModLoaderType::curseforge)
        ui->loaderSelect->addItem(ModLoaderType::icon(type), ModLoaderType::toString(type));

    connect(ui->modListWidget->verticalScrollBar(), &QAbstractSlider::valueChanged,  this , &CurseforgeModBrowser::onSliderChanged);
    connect(ui->searchText, &QLineEdit::returnPressed, this, &CurseforgeModBrowser::search);

    updateVersionList();
    connect(VersionManager::manager(), &VersionManager::curseforgeVersionListUpdated, this, &CurseforgeModBrowser::updateVersionList);

    updateCategoryList();

    updateLocalPathList();
    connect(LocalModPathManager::manager(), &LocalModPathManager::pathListUpdated, this, &CurseforgeModBrowser::updateLocalPathList);

    getModList(currentName_);
    isUiSet_ = true;
    connect(this, &CurseforgeModBrowser::downloadPathChanged, fileListWidget_, &CurseforgeFileListWidget::setDownloadPath);
}

CurseforgeModBrowser::~CurseforgeModBrowser()
{
    delete ui;
}

QWidget *CurseforgeModBrowser::infoWidget() const
{
    return infoWidget_;
}

QWidget *CurseforgeModBrowser::fileListWidget() const
{
    return fileListWidget_;
}

void CurseforgeModBrowser::refresh()
{
    search();
}

void CurseforgeModBrowser::searchModByPathInfo(const LocalModPathInfo &info)
{
    isUiSet_ = false;
    currentGameVersion_ = info.gameVersion();
    ui->versionSelectButton->setText(info.gameVersion());
    ui->loaderSelect->setCurrentIndex(ModLoaderType::curseforge.indexOf(info.loaderType()));
    isUiSet_ = true;
    ui->downloadPathSelect->setCurrentText(info.displayName());
    getModList(currentName_);
}

void CurseforgeModBrowser::updateUi()
{
    for(int i = 0; i < ui->modListWidget->count(); i++){
        auto item = ui->modListWidget->item(i);
        if(!item->text().isEmpty()) continue;
        auto widget = ui->modListWidget->itemWidget(item);
        dynamic_cast<CurseforgeModItemWidget*>(widget)->updateUi();
    }
}

void CurseforgeModBrowser::updateVersionList()
{
    auto menu = new QMenu;
    menu->setStyleSheet("QMenu { menu-scrollable: 1; }");
    auto anyVersionAction = menu->addAction(tr("Any"));
    connect(anyVersionAction, &QAction::triggered, this, [=]{
        currentGameVersion_ = GameVersion::Any;
        ui->versionSelectButton->setText(tr("Any"));
        ui->versionSelectButton->setIcon(QIcon());
    });
    anyVersionAction->trigger();
    menu->addSeparator();
    QMap<QString, QMenu*> submenus;
    QList<QString> keys; //to keep order
    for(auto &&version : GameVersion::curseforgeVersionList()){
        if(!submenus.contains(version.majorVersion())){
            auto submenu = new QMenu(version.majorVersion());
            submenus[version.majorVersion()] = submenu;
            keys << version.majorVersion();
        }
    }
    for(auto &&version : GameVersion::curseforgeVersionList()){
        if(submenus.contains(version.majorVersion())){
            auto submenu = submenus[version.majorVersion()];
            if(submenu->actions().size() == 1)
                if(GameVersion version = submenu->actions().at(0)->text(); version == version.majorVersion())
                    submenu->addSeparator();
            connect(submenu->addAction(version), &QAction::triggered, this, [=]{
                currentGameVersion_ = version;
                ui->versionSelectButton->setText(version);
            });
        }
    }
    for(const auto &key : qAsConst(keys)){
        auto submenu = submenus[key];
        if(submenu->actions().size() == 1)
            menu->addActions(submenu->actions());
        else
            menu->addMenu(submenu);
    }
    connect(menu, &QMenu::triggered, this, [=]{
        getModList(currentName_);
    });
    ui->versionSelectButton->setMenu(menu);
}

void CurseforgeModBrowser::updateCategoryList()
{
    auto menu = new QMenu;
    auto anyCategoryAction = menu->addAction(tr("Any"));
    connect(anyCategoryAction, &QAction::triggered, this, [=]{
        currentCategoryId_ = 0;
        ui->categorySelectButton->setText(tr("Any"));
        ui->categorySelectButton->setIcon(QIcon());
    });
    anyCategoryAction->trigger();
    menu->addSeparator();
    QMap<int, QMenu*> submenus;
    for(auto &&[id, name, iconName, parentId] : CurseforgeAPI::getCategories()){
        if(parentId == 6){
            auto submenu = new QMenu(name);
            submenus[id] = submenu;
            submenu->setIcon(QIcon(QString(":/image/curseforge/%1.png").arg(iconName)));
            QIcon icon(QString(":/image/curseforge/%1.png").arg(iconName));
            connect(submenu->addAction(icon, name), &QAction::triggered, this, [=, id = id, name = name]{
                currentCategoryId_ = id;
                ui->categorySelectButton->setText(name);
                ui->categorySelectButton->setIcon(icon);
            });
        }
    }
    for(auto &&[id, name, iconName, parentId] : CurseforgeAPI::getCategories()){
        if(submenus.contains(parentId)){
            auto submenu = submenus[parentId];
            if(submenu->actions().size() == 1)
                submenu->addSeparator();
            QIcon icon(QString(":/image/curseforge/%1.png").arg(iconName));
            connect(submenu->addAction(icon, name), &QAction::triggered, this, [=, id = id, name = name]{
                currentCategoryId_ = id;
                ui->categorySelectButton->setText(name);
                ui->categorySelectButton->setIcon(icon);
            });
        }
    }
    for(auto submenu : qAsConst(submenus)){
        if(submenu->actions().size() == 1)
            menu->addActions(submenu->actions());
        else
            menu->addMenu(submenu);
    }
    connect(menu, &QMenu::triggered, this, [=]{
        getModList(currentName_);
    });
    ui->categorySelectButton->setMenu(menu);
}

void CurseforgeModBrowser::updateLocalPathList()
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

void CurseforgeModBrowser::search()
{
//    if(ui->searchText->text() == currentName_) return;
    currentName_ = ui->searchText->text();
    getModList(currentName_);
}

void CurseforgeModBrowser::onSliderChanged(int i)
{
    if(!isSearching_ && hasMore_ && i >= ui->modListWidget->verticalScrollBar()->maximum() - 1000){
        currentIndex_ += Config().getSearchResultCount();
        getModList(currentName_, currentIndex_);
    }
}

void CurseforgeModBrowser::getModList(QString name, int index, int needMore)
{
    if(!index)
        currentIndex_ = 0;
    else if(!hasMore_ || isSearching_)
        return;
    setCursor(Qt::BusyCursor);

    GameVersion gameVersion = currentGameVersion_;
    auto category = currentCategoryId_;
    auto sort = ui->sortSelect->currentIndex();

    isSearching_ = true;
    auto conn = api_->searchMods(gameVersion, index, name, category, sort, [=](const QList<CurseforgeModInfo> &infoList){
        setCursor(Qt::ArrowCursor);

        //new search
        if(currentIndex_ == 0){
            idList_.clear();
            ui->modListWidget->clear();
            hasMore_ = true;
        }

        //show them
        int shownCount = 0;
        for(const auto &info : qAsConst(infoList)){
            //do not load duplicate mod
            if(idList_.contains(info.id()))
                continue;
            idList_ << info.id();

            auto mod = new CurseforgeMod(this, info);
            auto *listItem = new QListWidgetItem();
            listItem->setSizeHint(QSize(0, 108));
            auto loaderType = ModLoaderType::curseforge.at(ui->loaderSelect->currentIndex());
            auto fileInfo = mod->modInfo().latestFileInfo(currentGameVersion_, loaderType);
            auto modItemWidget = new CurseforgeModItemWidget(ui->modListWidget, mod, fileInfo);
            mod->setParent(modItemWidget);
            modItemWidget->setDownloadPath(downloadPath_);
            connect(this, &CurseforgeModBrowser::downloadPathChanged, modItemWidget, &CurseforgeModItemWidget::setDownloadPath);
            ui->modListWidget->addItem(listItem);
            ui->modListWidget->setItemWidget(listItem, modItemWidget);
            auto isShown = loaderType == ModLoaderType::Any || info.loaderTypes().contains(loaderType);
            if(info.loaderTypes().isEmpty())
                isShown = true;
            listItem->setHidden(!isShown);
            if(isShown){
                shownCount++;
                mod->acquireIcon();
            }
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
        if(hasMore_ && shownCount != infoList.count() && shownCount < needMore){
            currentIndex_ += 20;
            getModList(currentName_, currentIndex_, needMore - shownCount);
        }
    });
    connect(this, &QObject::destroyed, this, [=]{
        disconnect(conn);
    });
}

void CurseforgeModBrowser::on_modListWidget_doubleClicked(const QModelIndex &index)
{
    auto item = ui->modListWidget->item(index.row());
    if(item->data(Qt::UserRole + 1).toBool()) return;
    item->setData(Qt::UserRole + 1, true);
    if(!item->text().isEmpty()) return;
    auto widget = dynamic_cast<CurseforgeModItemWidget*>(ui->modListWidget->itemWidget(item));
    auto mod = widget->mod();
    auto dialog = new CurseforgeModDialog(this, mod);
    //set parent
    mod->setParent(dialog);
    dialog->setDownloadPath(downloadPath_);
    connect(this, &CurseforgeModBrowser::downloadPathChanged, dialog, &CurseforgeModDialog::setDownloadPath);
    connect(dialog, &CurseforgeModDialog::finished, widget, [=]{
        mod->setParent(widget);
        item->setData(Qt::UserRole + 1, false);
    });
    dialog->show();
}

void CurseforgeModBrowser::on_sortSelect_currentIndexChanged(int)
{
    if(isUiSet_) getModList(currentName_);
}

void CurseforgeModBrowser::on_loaderSelect_currentIndexChanged(int index)
{
    for(int i = 0; i < ui->modListWidget->count(); i++){
        auto item = ui->modListWidget->item(i);
        if(!item->text().isEmpty()) continue;
        auto isHidden = item->isHidden();
        auto widget = ui->modListWidget->itemWidget(item);
        auto mod = dynamic_cast<CurseforgeModItemWidget*>(widget)->mod();
        auto selectedLoaderType = ModLoaderType::curseforge.at(index);
        auto isShown = selectedLoaderType == ModLoaderType::Any || mod->modInfo().loaderTypes().contains(selectedLoaderType);
        if(mod->modInfo().loaderTypes().isEmpty())
            isShown = true;
        item->setHidden(!isShown);
        //hidden -> shown, while not have downloaded thumbnail yet
        if(isHidden && isShown && mod->modInfo().iconBytes().isEmpty())
            mod->acquireIcon();
    }
}

void CurseforgeModBrowser::on_downloadPathSelect_currentIndexChanged(int index)
{
    if(!isUiSet_ || index < 0 || index >= ui->downloadPathSelect->count()) return;
    if(index == 0)
        downloadPath_ = nullptr;
    else
        downloadPath_ =  LocalModPathManager::pathList().at(index - 1);
    emit downloadPathChanged(downloadPath_);
}


void CurseforgeModBrowser::on_openFolderButton_clicked()
{
    QString path;
    if(downloadPath_)
        path = downloadPath_->info().path();
    else
        path = Config().getDownloadPath();
    openFileInFolder(path);
}

void CurseforgeModBrowser::on_modListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous[[maybe_unused]])
{
    if(!current || !current->text().isEmpty()) return;
    auto widget = ui->modListWidget->itemWidget(current);
    auto mod = dynamic_cast<CurseforgeModItemWidget*>(widget)->mod();
    infoWidget_->setMod(mod);
    fileListWidget_->setMod(mod);
}
