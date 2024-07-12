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
#include <QProcess>
#include <QToolBar>
#include <QWindow>
#ifdef DE_KDE
#include <KWindowEffects>
#endif

#include "ui/local/importcurseforgemodpackdialog.h"
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
#include "ui/github/githubrepobrowser.h"
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
#include "ui/modmanagerupdatedialog.h"

const GitHubRepoInfo &ModManager::githubInfo()
{
    static GitHubRepoInfo info("kaniol-lck", "modmanager", tr("Mod Manager"), QIcon(":/image/modmanager.png"));
    return info;
}

ModManager::ModManager(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ModManager),
    browserSelector_(new BrowserSelectorWidget(this))
{
    Config config;
    enableBlurBehind_ = config_.getEnableBlurBehind();
    ui->setupUi(this);
    menuBar_ = new QMenuBar(this);
    menuBar_->addActions(ui->menubar->actions());
    menuBar_->hide();
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
    for(auto &&widget : { ui->pageSelectorDock, ui->modInfoDock, ui->fileListDock })
        ui->menu_View->insertAction(ui->menu_View->actions().at(0), widget->toggleViewAction());

    //Download
    pageSwitcher_.addDownloadPage();
    //Explore
    if(config.getShowCurseforge())
        pageSwitcher_.addExploreBrowser(new CurseforgeModBrowser(this));
    if(config.getShowModrinth())
        pageSwitcher_.addExploreBrowser(new ModrinthModBrowser(this));
    if(config.getShowOptiFine())
        pageSwitcher_.addExploreBrowser(new OptifineModBrowser(this));
    if(config.getShowReplayMod())
        pageSwitcher_.addExploreBrowser(new ReplayModBrowser(this));

    pageSwitcher_.addExploreBrowser(new GitHubRepoBrowser(this, githubInfo()));

    //Local
    pageSwitcher_.syncPathList();
    connect(LocalModPathManager::manager(), &LocalModPathManager::pathListUpdated, &pageSwitcher_, &PageSwitcher::syncPathList);

    //default browser
    if(pageSwitcher_.model()->item(PageSwitcher::Explore)->hasChildren())
        pageSwitcher_.setPage(PageSwitcher::Explore, 0);

    //init versions
    VersionManager::initVersionLists();

    if(config_.getAutoCheckModManagerUpdate())
        ui->actionCheck_Mod_Manager_Update->trigger();

    ui->actionShow_Mod_Authors->setChecked(config.getShowModAuthors());
    ui->actionShow_Mod_Date_Time->setChecked(config.getShowModDateTime());
    ui->actionShow_Mod_Category->setChecked(config.getShowModCategory());
    ui->actionShow_Mod_Loader_Type->setChecked(config.getShowModLoaderType());

    mergeMenuBar();
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

void ModManager::mergeMenuBar()
{
    ui->menubar->clear();
    QMenu *menuPath;
    for(auto &&menuAction : menuBar_->actions()){
        auto menu = ui->menubar->addMenu(menuAction->icon(), menuAction->text());
        connect(menu, &QMenu::aboutToShow, menuAction->menu(), &QMenu::aboutToShow);
        connect(menu, &QMenu::aboutToHide, menuAction->menu(), &QMenu::aboutToHide);
        if(menuAction->menu() == ui->menu_Path)
            menuPath = menu;
        menu->addActions(menuAction->menu()->actions());
    }
    if(auto browser = pageSwitcher_.currentBrowser())
        for(auto &&menuAction : browser->menuBar()->actions()){
            bool merged = false;
            for(auto &&existedMenuAction : ui->menubar->actions())
                //merge if has same text
                if(existedMenuAction->text() == menuAction->text()){
                    if(!existedMenuAction->menu()->isEmpty())
                        existedMenuAction->menu()->addSeparator();
                    existedMenuAction->menu()->addActions(menuAction->menu()->actions());
                    merged = true;
                    break;
                }
            if(!merged){
                auto menu = new QMenu(menuAction->text());
                ui->menubar->insertMenu(menuPath->menuAction(), menu);
                menu->addActions(menuAction->menu()->actions());
            }
        }
    emit menuBarChanged();
}

void ModManager::updateBrowsers(Browser *previous[[maybe_unused]], Browser *current)
{
    setWindowTitle(current->name());
    ui->modInfoDock->setWidget(current->infoWidget());
    ui->fileListDock->setWidget(current->fileListWidget());
    mergeMenuBar();
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
    emit closed();
    qDebug() << "closed";
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
    for(auto &&widget : findChildren<QDockWidget *>()){
        if(!widget->isVisible()) continue;
        auto rect = widget->rect();
        rect.translate(widget->pos());
        p.fillRect(rect, QBrush(QColor(255, 255, 255, 210)));
    }
}
#endif //defined (DE_KDE) || defined (Q_OS_WIN)

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
    // TODO: mass performance
    // auto style = config_.getCustomStyle();
    // connect(preferences, &Preferences::accepted, this, [=]{
    //     if(auto afterStyle = config_.getCustomStyle(); afterStyle != style)
    //         qApp->setStyleSheet(styleSheetPath(afterStyle));
    // });
    connect(preferences, &Preferences::accepted, this, &ModManager::updateUi, Qt::UniqueConnection);
    connect(preferences, &Preferences::accepted, this, &ModManager::setProxy, Qt::UniqueConnection);
    connect(preferences, &Preferences::accepted, QAria2::qaria2(), &QAria2::updateOptions, Qt::UniqueConnection);

#ifdef Q_OS_WIN
    if(config_.getUseFramelessWindow()){
        auto frameless = new FramelessWrapper(preferences);
        frameless->show();
    } else
#endif
        preferences->exec();
}

void ModManager::on_actionManage_Browser_triggered()
{
    auto dialog = new BrowserManagerDialog();
//    connect(dialog, &BrowserManagerDialog::accepted, &pageSwitcher_, &PageSwitcher::syncPathList);
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
        menu->addActions(exploreBrowser->menu()->actions());
    } else if(index.parent().row() == PageSwitcher::Local){
        // on one of local items
        auto localBrowser = pageSwitcher_.localModBrowser(index.row());
        connect(menu->addAction(QIcon::fromTheme("entry-edit"), tr("Edit")), &QAction::triggered, this, [=]{
            editLocalPath(index.row());
        });
        auto reloadAction = menu->addAction(QIcon::fromTheme("view-refresh"), tr("Reload"), [=]{
            localBrowser->reload();
        });
        if(localBrowser->isLoading()){
            reloadAction->setEnabled(false);
            connect(localBrowser, &LocalModBrowser::loadFinished, this, [=]{
                reloadAction->setEnabled(true);
            });
        }
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
        auto frameless = new FramelessWrapper(dialog);
        frameless->show();
    } else
#endif
        dialog->show();
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

void ModManager::on_menu_View_aboutToShow()
{
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
    for(int index = 0; index < pageSwitcher_.model()->item(PageSwitcher::Explore)->rowCount(); index++){
        auto browser = pageSwitcher_.exploreBrowser(index);
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
    if(auto window = QWindow::fromWinId(winId()))
        KWindowEffects::enableBlurBehind(window, config_.getEnableBlurBehind());
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

void ModManager::on_actionCurseforge_Modpack_triggered()
{
    auto fileName = QFileDialog::getOpenFileName(this, tr("Select a modpack"), Config().getCommonPath(), "Modpack (*.zip)");
    if(fileName.isEmpty()) return;
    auto dialog = new ImportCurseforgeModpackDialog(this, fileName);
    if(!dialog->fileValid())
        QMessageBox::information(this, tr("Invalid File"), tr("Selected file is not a modpack"));
    else
        dialog->show();
}

void ModManager::on_actionCheck_Mod_Manager_Update_triggered()
{
    auto dialog = new ModManagerUpdateDialog(this);
    dialog->checkVersion();
    connect(dialog, &ModManagerUpdateDialog::updateChecked, this, [=](bool hasUpdate){
        if(hasUpdate){
            ui->actionCheck_Mod_Manager_Update->setText(tr("Update Mod Manager: %1").arg(dialog->updateVersion()));
            dialog->show();
        } else
            ui->actionCheck_Mod_Manager_Update->setText(tr("Mod Manager is Latest"));
    });
    connect(dialog, &ModManagerUpdateDialog::updateConfirmed, this, [=](const GitHubFileInfo &fileInfo){
        auto downloader = DownloadManager::manager()->download(fileInfo);
        connect(downloader, &AbstractDownloader::finished, this, [=]{
            if (QMessageBox::Yes == QMessageBox::question(this, tr("Update Ready"), tr("Update Mod Manager to %1 now?").arg(dialog->updateVersion()))){
                qDebug() << downloader->info().filePath();
                //TODO: filePath
                QProcess().startDetached(downloader->info().filePath(), {});
                close();
            }
        });
    });
}
