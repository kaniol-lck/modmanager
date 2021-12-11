#include "curseforgemodbrowser.h"
#include "ui_curseforgemodbrowser.h"

#include <QScrollBar>
#include <QDir>
#include <QDebug>
#include <QAction>
#include <QMenu>
#include <QStandardItem>
#include <QStatusBar>

#include "curseforgemodinfowidget.h"
#include "curseforgefilelistwidget.h"
#include "ui/explorestatusbarwidget.h"
#include "local/localmodpathmanager.h"
#include "local/localmodpath.h"
#include "local/localmod.h"
#include "curseforge/curseforgemod.h"
#include "curseforge/curseforgeapi.h"
#include "curseforgemoditemwidget.h"
#include "curseforgemoddialog.h"
#include "gameversion.h"
#include "modloadertype.h"
#include "config.hpp"
#include "util/funcutil.h"
#include "util/smoothscrollbar.h"

CurseforgeModBrowser::CurseforgeModBrowser(QWidget *parent, LocalMod *mod) :
    ExploreBrowser(parent, QIcon(":/image/curseforge.svg"), "Curseforge", QUrl("https://www.curseforge.com/minecraft/mc-mods")),
    ui(new Ui::CurseforgeModBrowser),
    model_(new QStandardItemModel(this)),
    infoWidget_(new CurseforgeModInfoWidget(this)),
    fileListWidget_(new CurseforgeFileListWidget(this)),
    statusBarWidget_(new ExploreStatusBarWidget(this)),
    statusBar_(new QStatusBar(this)),
    api_(new CurseforgeAPI(this)),
    localMod_(mod)
{
    infoWidget_->hide();
    fileListWidget_->hide();
    ui->setupUi(this);
    ui->modListView->setModel(model_);
    ui->modListView->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->modListView->setProperty("class", "ModList");

    //setup status bar
    statusBar_->addPermanentWidget(statusBarWidget_);
    layout()->addWidget(statusBar_);

    ui->loaderSelect->blockSignals(true);
    for(const auto &type : ModLoaderType::curseforge)
        ui->loaderSelect->addItem(ModLoaderType::icon(type), ModLoaderType::toString(type));
    ui->loaderSelect->blockSignals(false);

    updateVersionList();
    updateCategoryList();
    updateLocalPathList();
    updateStatusText();

    connect(ui->modListView->verticalScrollBar(), &QAbstractSlider::valueChanged,  this , &CurseforgeModBrowser::onSliderChanged);
    connect(ui->searchText, &QLineEdit::returnPressed, this, &CurseforgeModBrowser::search);
    connect(VersionManager::manager(), &VersionManager::curseforgeVersionListUpdated, this, &CurseforgeModBrowser::updateVersionList);
    connect(LocalModPathManager::manager(), &LocalModPathManager::pathListUpdated, this, &CurseforgeModBrowser::updateLocalPathList);
    connect(this, &CurseforgeModBrowser::downloadPathChanged, fileListWidget_, &CurseforgeFileListWidget::setDownloadPath);
    connect(ui->modListView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &CurseforgeModBrowser::onItemSelected);
    connect(ui->modListView->verticalScrollBar(), &QScrollBar::valueChanged, this, &CurseforgeModBrowser::updateIndexWidget);

    if(localMod_){
        currentName_ = localMod_->commonInfo()->id();
        ui->searchText->setText(currentName_);
    } else if(Config().getSearchModsOnStartup()){
        inited_ = true;
        getModList(currentName_);
    }
}

CurseforgeModBrowser::~CurseforgeModBrowser()
{
    delete ui;
    for(auto row = 0; row < model_->rowCount(); row++){
        auto item = model_->item(row);
        auto mod = item->data().value<CurseforgeMod*>();
        if(mod && !mod->parent())
            mod->deleteLater();
    }
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
    currentGameVersion_ = info.gameVersion();
    ui->versionSelectButton->setText(info.gameVersion());
    currentLoaderType_ = info.loaderType();
    ui->loaderSelect->setCurrentIndex(ModLoaderType::curseforge.indexOf(info.loaderType()));
    ui->downloadPathSelect->blockSignals(true);
    ui->downloadPathSelect->setCurrentText(info.displayName());
    ui->downloadPathSelect->blockSignals(false);
    getModList(currentName_);
}

void CurseforgeModBrowser::updateUi()
{
    for(int i = 0; i < model_->rowCount(); i++){
        if(auto widget = ui->modListView->indexWidget(model_->index(i, 0)))
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
    ui->downloadPathSelect->blockSignals(true);
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
    ui->downloadPathSelect->blockSignals(false);
}

void CurseforgeModBrowser::search()
{
//    if(ui->searchText->text() == currentName_) return;
    currentName_ = ui->searchText->text();
    getModList(currentName_);
}

void CurseforgeModBrowser::onSliderChanged(int i)
{
    if(!isSearching_ && hasMore_ && i >= ui->modListView->verticalScrollBar()->maximum() - 1000){
        currentIndex_ += Config().getSearchResultCount();
        getModList(currentName_, currentIndex_);
    }
}

void CurseforgeModBrowser::updateStatusText()
{
    auto str = tr("Loaded %1 mods from Curseforge.").arg(model_->rowCount());
    statusBar_->showMessage(str);
}

void CurseforgeModBrowser::getModList(QString name, int index, int needMore)
{
    if(!index)
        currentIndex_ = 0;
    else if(!hasMore_ || isSearching_)
        return;
    setCursor(Qt::BusyCursor);
    statusBarWidget_->setText(tr("Searching mods..."));
    statusBarWidget_->setProgressVisible(true);

    GameVersion gameVersion = currentGameVersion_;
    auto category = currentCategoryId_;
    auto sort = ui->sortSelect->currentIndex();

    isSearching_ = true;
    auto conn = api_->searchMods(gameVersion, index, name, category, sort, [=](const QList<CurseforgeModInfo> &infoList){
        setCursor(Qt::ArrowCursor);
        statusBarWidget_->setText("");
        statusBarWidget_->setProgressVisible(false);

        //new search
        if(currentIndex_ == 0){
            idList_.clear();
            for(auto row = 0; row < model_->rowCount(); row++){
                auto item = model_->item(row);
                auto mod = item->data().value<CurseforgeMod*>();
                if(mod && !mod->parent())
                    mod->deleteLater();
            }
            model_->clear();
            hasMore_ = true;
        }

        //show them
        int shownCount = 0;
        for(const auto &info : qAsConst(infoList)){
            //do not load duplicate mod
            if(idList_.contains(info.id()))
                continue;
            idList_ << info.id();

            auto mod = new CurseforgeMod((QObject*)nullptr, info);
            auto *item = new QStandardItem();
            item->setData(QVariant::fromValue(mod));
            auto fileInfo = mod->modInfo().latestFileInfo(currentGameVersion_, currentLoaderType_);
            item->setSizeHint(QSize(0, 100));
            model_->appendRow(item);
            auto isShown = currentLoaderType_ == ModLoaderType::Any || info.loaderTypes().contains(currentLoaderType_);
            if(info.loaderTypes().isEmpty())
                isShown = true;
            ui->modListView->setRowHidden(item->row(), !isShown);
            if(isShown){
                shownCount++;
                mod->acquireIcon();
            }
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
        if(hasMore_ && shownCount != infoList.count() && shownCount < needMore){
            currentIndex_ += 20;
            getModList(currentName_, currentIndex_, needMore - shownCount);
        }
        updateStatusText();
    });
    connect(this, &QObject::destroyed, this, [=]{
        disconnect(conn);
    });
}

void CurseforgeModBrowser::on_modListView_doubleClicked(const QModelIndex &index)
{
    auto item = model_->itemFromIndex(index);
    auto mod = item->data().value<CurseforgeMod*>();
    if(mod && !mod->parent()){
        auto dialog = new CurseforgeModDialog(this, mod);
        //set parent
        mod->setParent(dialog);
        dialog->setDownloadPath(downloadPath_);
        connect(this, &CurseforgeModBrowser::downloadPathChanged, dialog, &CurseforgeModDialog::setDownloadPath);
        connect(dialog, &CurseforgeModDialog::finished, this, [=]{
            mod->setParent(nullptr);
        });
        dialog->show();
    }
}

void CurseforgeModBrowser::on_loaderSelect_currentIndexChanged(int index)
{
    currentLoaderType_ = ModLoaderType::curseforge.at(index);
    for(int row = 0; row < model_->rowCount(); row++){
        auto item = model_->item(row);
        auto mod = item->data().value<CurseforgeMod*>();
        auto isShown = currentLoaderType_ == ModLoaderType::Any || mod->modInfo().loaderTypes().contains(currentLoaderType_);
        if(mod->modInfo().loaderTypes().isEmpty())
            isShown = true;
        bool isHidden = ui->modListView->isRowHidden(row);
        ui->modListView->setRowHidden(row, !isShown);
        //hidden -> shown, while not have downloaded thumbnail yet
        if(isHidden && isShown && mod->modInfo().iconBytes().isEmpty())
            mod->acquireIcon();
    }
}

void CurseforgeModBrowser::on_downloadPathSelect_currentIndexChanged(int index)
{
    if(index < 0 || index >= ui->downloadPathSelect->count()) return;
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

void CurseforgeModBrowser::onItemSelected()
{
    auto indexes = ui->modListView->selectionModel()->selectedRows();
    if(indexes.isEmpty()) return;
    auto index = indexes.first();
    auto item = model_->itemFromIndex(index);
    auto mod = item->data().value<CurseforgeMod*>();
    if(mod){
        infoWidget_->setMod(mod);
        fileListWidget_->setMod(mod);
        if(mod != selectedMod_){
            selectedMod_ = mod;
            emit selectedModsChanged(mod);
        }
    }
}

void CurseforgeModBrowser::updateIndexWidget()
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
        auto item = model_->item(row);
        auto mod = item->data().value<CurseforgeMod*>();
        if(mod){
            auto fileInfo = mod->modInfo().latestFileInfo(currentGameVersion_, currentLoaderType_);
            auto modItemWidget = new CurseforgeModItemWidget(ui->modListView, mod, fileInfo);
            modItemWidget->setDownloadPath(downloadPath_);
            connect(this, &CurseforgeModBrowser::downloadPathChanged, modItemWidget, &CurseforgeModItemWidget::setDownloadPath);
            ui->modListView->setIndexWidget(index, modItemWidget);
        }
    }
}

void CurseforgeModBrowser::paintEvent(QPaintEvent *event)
{
    if(!inited_){
        inited_ = true;
        getModList(currentName_);
    }
    updateIndexWidget();
    QWidget::paintEvent(event);
}

CurseforgeMod *CurseforgeModBrowser::selectedMod() const
{
//    qDebug() << selectedMod_;
    return selectedMod_;
}

QList<QAction *> CurseforgeModBrowser::modActions() const
{
    return modMenu_->actions();
}

QList<QAction *> CurseforgeModBrowser::pathActions() const
{
    return pathMenu_->actions();
}

ExploreBrowser *CurseforgeModBrowser::another()
{
    return new CurseforgeModBrowser;
}

void CurseforgeModBrowser::on_sortSelect_currentIndexChanged(int)
{
    getModList(currentName_);
}
