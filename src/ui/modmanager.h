#ifndef MODMANAGER_H
#define MODMANAGER_H

#include <QMainWindow>
#include <QTreeWidgetItem>

#include "local/localmodpathinfo.h"
#include "browserselectorwidget.h"

class QListWidgetItem;
class LocalModPath;
class BrowserSelectorWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class ModManager; }
QT_END_NAMESPACE

class ModManager : public QMainWindow
{
    Q_OBJECT

public:
    ModManager(QWidget *parent = nullptr);
    ~ModManager();

    void resizeEvent(QResizeEvent *event);
private slots:
    void syncPathList();
    void editLocalPath(int index);
    void on_actionPreferences_triggered();
    void on_actionManage_Browser_triggered();
    void customContextMenuRequested(const QModelIndex &index, const QPoint &pos);
    void on_action_About_Mod_Manager_triggered();
    void on_action_Browsers_toggled(bool arg1);
    void on_actionOpen_new_path_dialog_triggered();
    void on_actionSelect_A_Directory_triggered();
    void on_actionSelect_Multiple_Directories_triggered();
    void on_menu_Path_aboutToShow();
    void on_menu_Help_aboutToShow();
    void on_menuPaths_aboutToShow();
    void on_menuTags_aboutToShow();
    void on_actionReload_triggered();
    void on_actionShow_Mod_Date_Time_toggled(bool arg1);
    void on_actionShow_Mod_Category_toggled(bool arg1);
    void on_actionShow_Mod_Loader_Type_toggled(bool arg1);
private:
    Ui::ModManager *ui;
    BrowserSelectorWidget *browserSelector_;

    QList<LocalModPath*> pathList_;
};
#endif // MODMANAGER_H
