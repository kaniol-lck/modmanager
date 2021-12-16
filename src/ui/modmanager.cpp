#include "framelesswrapper.h"
#include "modmanager.h"
#include "ui_modmanager.h"

#include <QDebug>
#include <QUrl>
#include <QDesktopServices>
#include <QMessageBox>
#include <QResizeEvent>
#include <QFileDialog>
#include <QLabel>
#include <QToolButton>
#include <QPainter>
#include <QNetworkProxy>
#ifdef DE_KDE
#include <KWindowEffects>
#endif

#include "dockwidgetcontent.h"
#include "ui/aboutdialog.h"
#include "ui/browserselectorwidget.h"
#include "local/localmodpathmanager.h"
#include "local/localmodpath.h"
#include "ui/local/localmodbrowser.h"
#include "ui/curseforge/curseforgemodbrowser.h"
#include "ui/modrinth/modrinthmodbrowser.h"
#include "ui/optifine/optifinemodbrowser.h"
#include "ui/replay/replaymodbrowser.h"
#include "ui/download/downloadbrowser.h"
#include "ui/preferences.h"
#include "ui/browsermanagerdialog.h"
#include "ui/local/localmodpathsettingsdialog.h"
#include "download/qaria2.h"
#include "gameversion.h"
#include "config.hpp"
#include "util/funcutil.h"
#include "ui/windowstitlebar.h"
#include "qss/stylesheets.h"
#include "ui/browserdialog.h"
#include "local/knownfile.h"

ModManager::ModManager(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ModManager),
    browserSelector_(new BrowserSelectorWidget(this))
{
    Config config;
    enableBlurBehind_ = config_.getEnableBlurBehind();
    ui->setupUi(this);
    setCentralWidget(&pageSwitcher_);
    DockWidgetContent::lockPanelsAction = ui->actionLock_Panels;
    restoreGeometry(config.getGeometry());
    restoreState(config.getWindowState());
    ui->actionAbout_Qt->setIcon(QIcon(":/qt-project.org/qmessagebox/images/qtlogo-64.png"));
    browserSelector_->setModel(pageSwitcher_.model());
    connect(browserSelector_, &BrowserSelectorWidget::browserChanged, &pageSwitcher_, &PageSwitcher::setPage);
    connect(&pageSwitcher_, &PageSwitcher::pageChanged, browserSelector_, &BrowserSelectorWidget::setCurrentIndex);
    connect(&pageSwitcher_, &PageSwitcher::browserChanged, this, &ModManager::updateBrowsers);
    connect(browserSelector_, &BrowserSelectorWidget::customContextMenuRequested, this, &ModManager::customContextMenuRequested);
    LocalModPathManager::load();

    setProxy();
    updateLockPanels();

    ui->pageSelectorDock->setWidget(browserSelector_);

    //Download
    pageSwitcher_.addDownloadPage();
    //Explore
    if(config.getShowCurseforge()) pageSwitcher_.addCurseforgePage();
    if(config.getShowModrinth()) pageSwitcher_.addModrinthPage();
    if(config.getShowOptiFine()) pageSwitcher_.addOptiFinePage();
    if(config.getShowReplayMod()) pageSwitcher_.addReplayModPage();

    pageSwitcher_.addGitHubPage(GitHubRepoInfo("kaniol-lck", "modmanager", windowTitle(), QIcon(":/image/modmanager.png")));

    //Local
    syncPathList();
    connect(LocalModPathManager::manager(), &LocalModPathManager::pathListUpdated, this, &ModManager::syncPathList);

    //default browser
    if(pageSwitcher_.exploreBrowsers().size())
        pageSwitcher_.setPage(PageSwitcher::Explore, 0);

    //init versions
    VersionManager::initVersionLists();

    ui->actionShow_Mod_Authors->setChecked(config.getShowModAuthors());
    ui->actionShow_Mod_Date_Time->setChecked(config.getShowModDateTime());
    ui->actionShow_Mod_Category->setChecked(config.getShowModCategory());
    ui->actionShow_Mod_Loader_Type->setChecked(config.getShowModLoaderType());

    updateUi();
}

ModManager::~ModManager()
{
    Config config;
    config.setGeometry(saveGeometry());
    config.setWindowState(saveState());
    delete ui;
}

void ModManager::updateUi()
{
    qApp->setStyleSheet(styleSheetPath(config_.getCustomStyle()));
    pageSwitcher_.updateUi();
    updateBlur();
}

void ModManager::setProxy()
{
    QNetworkProxy proxy;
    proxy.setType(static_cast<QNetworkProxy::ProxyType>(config_.getProxyType()));
    proxy.setHostName(config_.getProxyHostName());
    proxy.setPort(config_.getProxyPort());
    proxy.setUser(config_.getProxyUser());
    proxy.setPassword(config_.getProxyPassword());
    QNetworkProxy::setApplicationProxy(proxy);
}

void ModManager::updateBrowsers(Browser *previous, Browser *current)
{
    ui->modInfoDock->setWidget(current->infoWidget());
    ui->fileListDock->setWidget(current->fileListWidget());
    ui->menu_Mod->clear();
    ui->menu_Mod->addActions(current->modActions());
    ui->menu_Mod->setEnabled(!current->modActions().isEmpty());
    for(const auto &action : previous->pathActions())
        ui->menu_Path->removeAction(action);
    ui->menu_Path->addActions(current->pathActions());
}

void ModManager::updateLockPanels()
{
    if(config_.getLockPanel()){
        ui->actionLock_Panels->setIcon(QIcon::fromTheme("unlock"));
        ui->actionLock_Panels->setText(tr("Unlock Panels"));
        for(auto &&widget : { ui->pageSelectorDock, ui->modInfoDock, ui->fileListDock }){
            widget->setFeatures(QDockWidget::NoDockWidgetFeatures);
            widget->setTitleBarWidget(new QLabel);
        }
    } else{
        ui->actionLock_Panels->setIcon(QIcon::fromTheme("lock"));
        ui->actionLock_Panels->setText(tr("Lock Panels"));
        for(auto &&widget : { ui->pageSelectorDock, ui->modInfoDock, ui->fileListDock }){
            widget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
            widget->setTitleBarWidget(nullptr);
        }
    }
}

void ModManager::closeEvent(QCloseEvent *event[[maybe_unused]])
{
//    qDebug() << "closed";
}

#if defined (DE_KDE) || defined (Q_OS_WIN)
void ModManager::paintEvent(QPaintEvent *event[[maybe_unused]])
{
    if(!enableBlurBehind_) return;
#ifdef DE_KDE
    if(!KWindowEffects::isEffectAvailable(KWindowEffects::BlurBehind))
        return;
#endif
    QPainter p(this);
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    for(auto &&widget : { ui->pageSelectorDock, ui->modInfoDock, ui->fileListDock }){
        if(!widget->isVisible()) continue;
        auto rect = widget->rect();
        rect.translate(widget->pos());
        p.fillRect(rect, QBrush(QColor(255, 255, 255, 191)));
    }
#ifdef Q_OS_WIN
//    if(titleBar_){
//        auto rect2 = titleBar_->rect();
//        rect2.translate(titleBar_->pos());
//        p.fillRect(rect2, QBrush(QColor(255, 255, 255, 127)));
//    }
#endif //Q_OS_WIN
}
#endif //defined (DE_KDE) || defined (Q_OS_WIN)

void ModManager::syncPathList()
{
    //remember selected path
    LocalModBrowser *selectedBrowser = nullptr;
    if(pageSwitcher_.currentCategory() == PageSwitcher::Local)
        selectedBrowser = pageSwitcher_.localModBrowser(pageSwitcher_.currentPage());

    auto oldCount = pathList_.size();
    for(const auto &path : LocalModPathManager::pathList()){
        if(auto i = pathList_.indexOf(path); i < 0){
            //not present, new one
            pathList_ << path;
            pageSwitcher_.addLocalPage(path);
        } else{
            //present, move position
            oldCount--;
            auto path = pathList_.takeAt(i);
            pathList_ << path;
            auto browser = pageSwitcher_.takeLocalModBrowser(i);
            pageSwitcher_.addLocalPage(browser);
        }
    }
    //remove remained mod path
    auto i = oldCount;
    while (i--) {
        pathList_.removeAt(i);
        pageSwitcher_.removeLocalModBrowser(i);
    }

    //they should be same after sync
    assert(pathList_ == LocalModPathManager::pathList());

    //reset selected path
    if(!selectedBrowser){
        if(auto index = pageSwitcher_.localModBrowsers().indexOf(selectedBrowser); index >= 0)
            pageSwitcher_.setPage(PageSwitcher::Local, index);
    }
}

void ModManager::editLocalPath(int index)
{
    auto pathInfo = LocalModPathManager::pathList().at(index)->info();
    auto dialog = new LocalModPathSettingsDialog(this, pathInfo);
    connect(dialog, &LocalModPathSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &newInfo, bool autoLoaderType){
        LocalModPathManager::pathList().at(index)->setInfo(newInfo, autoLoaderType);
//        browserSelector_->localItem()->child(index)->setText(0, newInfo.displayName());
    });
    dialog->exec();
}

void ModManager::on_actionPreferences_triggered()
{
    auto preferences = new Preferences(this);
    preferences->exec();
    connect(preferences, &Preferences::accepted, this, &ModManager::updateUi, Qt::UniqueConnection);
    connect(preferences, &Preferences::accepted, this, &ModManager::setProxy, Qt::UniqueConnection);
    connect(preferences, &Preferences::accepted, QAria2::qaria2(), &QAria2::updateOptions, Qt::UniqueConnection);
}

void ModManager::on_actionManage_Browser_triggered()
{
    auto dialog = new BrowserManagerDialog();
    connect(dialog, &BrowserManagerDialog::accepted, this, &ModManager::syncPathList);
    dialog->show();
}

void ModManager::customContextMenuRequested(const QModelIndex &index, const QPoint &pos)
{
    auto menu = new QMenu(this);
    if(!index.isValid()){
        // in empty area
        connect(menu->addAction(QIcon::fromTheme("list-add"), tr("New Mod Path")), &QAction::triggered, this, [=]{
            auto dialog = new LocalModPathSettingsDialog(this);
            connect(dialog, &LocalModPathSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &pathInfo, bool autoLoaderType){
                auto path = new LocalModPath(pathInfo);
                path->loadMods(autoLoaderType);
                LocalModPathManager::addPath(path);
            });
            dialog->exec();
        });
        menu->addSeparator();
        menu->addAction(ui->actionManage_Browser);
        menu->addSeparator();
        menu->addAction(ui->actionLock_Panels);
    } else if(index.parent().row() == PageSwitcher::Explore){
        // on one of explore items
        auto exploreBrowser = pageSwitcher_.exploreBrowser(index.row());
        menu->addActions(exploreBrowser->pathActions());
    } else if(index.parent().row() == PageSwitcher::Local){
        // on one of local items
        auto localBrowser = pageSwitcher_.localModBrowser(index.row());
        connect(menu->addAction(QIcon::fromTheme("entry-edit"), tr("Edit")), &QAction::triggered, this, [=]{
            editLocalPath(index.row());
        });
        auto reloadAction = menu->addAction(QIcon::fromTheme("view-refresh"), tr("Reload"));
        if(localBrowser->isLoading()){
            reloadAction->setEnabled(false);
            connect(localBrowser, &LocalModBrowser::loadFinished, this, [=]{
                reloadAction->setEnabled(true);
            });
        }
        connect(reloadAction, &QAction::triggered, this, [=]{
            localBrowser->reload();
        });
        connect(menu->addAction(QIcon::fromTheme("delete"), tr("Delete")), &QAction::triggered, this, [=]{
            if(QMessageBox::No == QMessageBox::question(this, tr("Delete"), tr("Delete this mod path?"))) return;
            LocalModPathManager::removePathAt(index.row());
        });
        menu->addSeparator();
        menu->addAction(ui->actionManage_Browser);
    }
    if(!menu->actions().isEmpty())
        menu->exec(pos);
}

void ModManager::on_action_About_Mod_Manager_triggered()
{
    auto dialog = new AboutDialog(this);
#ifdef Q_OS_WIN
    if(config_.getUseFramelessWindow()){
        auto frameless = FramelessWrapper::makeFrameless(dialog);
        frameless->show();
    } else
#endif
        dialog->show();
}

void ModManager::on_actionPage_Selector_toggled(bool arg1)
{
    ui->pageSelectorDock->setVisible(arg1);
}

void ModManager::on_actionMod_Infomation_toggled(bool arg1)
{
    ui->modInfoDock->setVisible(arg1);
}

void ModManager::on_actionFile_List_toggled(bool arg1)
{
    ui->fileListDock->setVisible(arg1);
}

void ModManager::on_actionOpen_new_path_dialog_triggered()
{
    auto dialog = new LocalModPathSettingsDialog(this);
    connect(dialog, &LocalModPathSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &pathInfo, bool autoLoaderType){
        auto path = new LocalModPath(pathInfo);
        path->loadMods(autoLoaderType);
        LocalModPathManager::addPath(path);
    });
    dialog->exec();
}

void ModManager::on_actionSelect_A_Directory_triggered()
{
    auto pathStr = QFileDialog::getExistingDirectory(this, tr("Select your mod directory..."), config_.getCommonPath());
    if(pathStr.isEmpty()) return;
    auto path = new LocalModPath(LocalModPathInfo::deduceFromPath(pathStr));
    path->loadMods(true);
    LocalModPathManager::addPath(path);
}

void ModManager::on_actionSelect_Multiple_Directories_triggered()
{
    auto paths = getExistingDirectories(this, tr("Select your mod directories..."), config_.getCommonPath());
    LocalModPathManager::addPaths(paths);
}

void ModManager::on_menu_Mod_aboutToShow()
{
    pageSwitcher_.currentBrowser()->onModMenuAboutToShow();
}

void ModManager::on_menu_Mod_aboutToHide()
{
    pageSwitcher_.currentBrowser()->onModMenuAboutToHide();
}

void ModManager::on_menu_Path_aboutToShow()
{
    if(pageSwitcher_.currentCategory() == PageSwitcher::Download)
        ui->actionReload->setEnabled(false);
    if(pageSwitcher_.currentCategory() == PageSwitcher::Local)
        if(auto localBrowser = pageSwitcher_.localModBrowser(pageSwitcher_.currentPage()); localBrowser && localBrowser->isLoading()){
            ui->actionReload->setEnabled(false);
            connect(localBrowser, &LocalModBrowser::loadFinished, this, [=]{
                ui->actionReload->setEnabled(true);
            });
        }
}

void ModManager::on_menu_View_aboutToShow()
{
    ui->actionPage_Selector->setChecked(ui->pageSelectorDock->isVisible());
    ui->actionMod_Infomation->setChecked(ui->modInfoDock->isVisible());
    ui->actionFile_List->setChecked(ui->fileListDock->isVisible());

    ui->actionShow_Mod_Authors->setChecked(config_.getShowModAuthors());
    ui->actionShow_Mod_Category->setChecked(config_.getShowModCategory());
    ui->actionShow_Mod_Date_Time->setChecked(config_.getShowModDateTime());
    ui->actionShow_Mod_Game_Version->setChecked(config_.getShowModGameVersion());
    ui->actionShow_Mod_Loader_Type->setChecked(config_.getShowModLoaderType());
    ui->actionShow_Mod_Release_Type->setChecked(config_.getShowModReleaseType());
}

void ModManager::on_menu_Help_aboutToShow()
{
    for(auto &&action : ui->menu_Help->actions()){
        if(action->data().toBool())
            ui->menu_Help->removeAction(action);
    }
    int count = 0;
    for(auto browser : pageSwitcher_.exploreBrowsers()){
        auto action = browser->visitWebsiteAction();
        action->setData(true);
        ui->menu_Help->insertAction(ui->menu_Help->actions().at(count++), action);
    }
}

void ModManager::on_menuPaths_aboutToShow()
{
    ui->menuPaths->clear();
    int index = 0;
    for(auto path : LocalModPathManager::pathList()){
        ui->menuPaths->addAction(path->info().icon(), path->info().displayName(), this, [=]{
            pageSwitcher_.setPage(PageSwitcher::Local, index);
        });
        index++;
    }
}

void ModManager::on_menuTags_aboutToShow()
{
    auto showTagCategories = config_.getShowTagCategories();
    ui->menuTags->clear();
    int count = 0;
    for(const auto &category : TagCategory::PresetCategories){
        auto action = new QAction(category.name());
        action->setCheckable(true);
        if(showTagCategories.contains(category)){
            action->setChecked(true);
            count++;
        }
        connect(action, &QAction::toggled, this, [=](bool bl){
            auto list = showTagCategories;
            if(bl)
                list.insert(count, category) ;
            else
                list.removeAll(category);
            config_.setShowTagCategories(list);
            pageSwitcher_.updateUi();
        });
        ui->menuTags->addAction(action);
    }
}

void ModManager::on_actionReload_triggered()
{
    if(auto category = pageSwitcher_.currentCategory(); category == PageSwitcher::Explore)
        pageSwitcher_.exploreBrowser(pageSwitcher_.currentPage())->refresh();
    else if(category == PageSwitcher::Local)
        pageSwitcher_.localModBrowser(pageSwitcher_.currentPage())->reload();
}

void ModManager::on_actionShow_Mod_Authors_toggled(bool arg1)
{
    config_.setShowModAuthors(arg1);
    pageSwitcher_.updateUi();
}

void ModManager::on_actionShow_Mod_Date_Time_toggled(bool arg1)
{
    config_.setShowModDateTime(arg1);
    pageSwitcher_.updateUi();
}

void ModManager::on_actionShow_Mod_Category_toggled(bool arg1)
{
    config_.setShowModCategory(arg1);
    pageSwitcher_.updateUi();
}

void ModManager::on_actionShow_Mod_Loader_Type_toggled(bool arg1)
{
    config_.setShowModLoaderType(arg1);
    pageSwitcher_.updateUi();
}

void ModManager::on_actionShow_Mod_Release_Type_toggled(bool arg1)
{
    config_.setShowModReleaseType(arg1);
    pageSwitcher_.updateUi();
}

void ModManager::on_actionShow_Mod_Game_Version_toggled(bool arg1)
{
    config_.setShowModGameVersion(arg1);
    pageSwitcher_.updateUi();
}

void ModManager::on_actionNext_Page_triggered()
{
    pageSwitcher_.nextPage();
}

void ModManager::on_actionPrevious_Page_triggered()
{
    pageSwitcher_.previesPage();
}

void ModManager::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void ModManager::updateBlur() const
{
#ifdef DE_KDE
    //TODO: disable blur under intel graphical card
    KWindowEffects::enableBlurBehind(windowHandle(), config_.getEnableBlurBehind());
#endif
}

void ModManager::on_actionLock_Panels_triggered()
{
    config_.setLockPanel(!config_.getLockPanel());
    updateLockPanels();
}

void ModManager::on_actionClear_Unmatched_File_Link_Caches_triggered()
{
    if(QMessageBox::No == QMessageBox::question(this, tr("Clear Caches"), tr("Clear unmatched file links?\n"
                                                                             "By doing this, we will recheck those files unmatched before."))) return;
    KnownFile::clearUnmatched();
}

