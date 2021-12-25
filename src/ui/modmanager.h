#ifndef MODMANAGER_H
#define MODMANAGER_H

#include <QMainWindow>
#include <QTreeWidgetItem>

#include "local/localmodpathinfo.h"
#include "browserselectorwidget.h"
#include "config.hpp"
#include "pageswitcher.h"

class QListWidgetItem;
class LocalModPath;
class BrowserSelectorWidget;
class WindowsTitleBar;
class Browser;

QT_BEGIN_NAMESPACE

namespace Ui { class ModManager; }
QT_END_NAMESPACE

class ModManager : public QMainWindow
{
    Q_OBJECT

public:
    ModManager(QWidget *parent = nullptr);
    ~ModManager();

protected:
    void closeEvent(QCloseEvent *event) override;
#if defined (DE_KDE) || defined (Q_OS_WIN)
    void paintEvent(QPaintEvent *event) override;
#endif //defined (DE_KDE) || defined (Q_OS_WIN)
public slots:
    void updateUi();
private slots:
    void setProxy();
    void updateBrowsers(Browser *previous, Browser *current);
    void updateLockPanels();
    void editLocalPath(int index);
    void on_actionPreferences_triggered();
    void on_actionManage_Browser_triggered();
    void customContextMenuRequested(const QModelIndex &index, const QPoint &pos);
    void on_action_About_Mod_Manager_triggered();
    void on_actionOpen_new_path_dialog_triggered();
    void on_actionSelect_A_Directory_triggered();
    void on_actionSelect_Multiple_Directories_triggered();
    void on_menu_Mod_aboutToShow();
    void on_menu_Mod_aboutToHide();
    void on_menu_Path_aboutToShow();
    void on_menu_View_aboutToShow();
    void on_menu_Help_aboutToShow();
    void on_menuPaths_aboutToShow();
    void on_menuTags_aboutToShow();
    void on_actionReload_triggered();
    void on_actionShow_Mod_Authors_toggled(bool arg1);
    void on_actionShow_Mod_Date_Time_toggled(bool arg1);
    void on_actionShow_Mod_Category_toggled(bool arg1);
    void on_actionShow_Mod_Loader_Type_toggled(bool arg1);
    void on_actionShow_Mod_Release_Type_toggled(bool arg1);
    void on_actionShow_Mod_Game_Version_toggled(bool arg1);
    void on_actionNext_Page_triggered();
    void on_actionPrevious_Page_triggered();
    void on_actionAbout_Qt_triggered();
//    void on_modInfoDock_customContextMenuRequested(const QPoint &pos);
//    void on_fileListDock_customContextMenuRequested(const QPoint &pos);
    void on_actionLock_Panels_triggered();
    void on_actionClear_Unmatched_File_Link_Caches_triggered();

private:
    Ui::ModManager *ui;
    PageSwitcher pageSwitcher_;
    mutable Config config_;
    BrowserSelectorWidget *browserSelector_;
    QList<LocalModPath*> pathList_;
    bool enableBlurBehind_;
    bool isMoving_ = false;
    void updateBlur() const;
};
#endif // MODMANAGER_H
