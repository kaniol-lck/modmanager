#ifndef LOCALMODBROWSER_H
#define LOCALMODBROWSER_H

#include "ui/browser.h"
#include "local/localmodpathinfo.h"

#include <QButtonGroup>
#include <QModelIndex>

class QListWidgetItem;
class QStandardItemModel;
class LocalMod;
class QStandardItem;
class QMenu;
class LocalModPath;
class LocalModFilter;
class QStatusBar;
class QProgressBar;
class LocalModInfoWidget;
class LocalFileListWidget;
class LocalStatusBarWidget;
class QAbstractItemView;
namespace Ui {
class LocalModBrowser;
}
class LocalModBrowser : public Browser
{
    Q_OBJECT
public:
    explicit LocalModBrowser(QWidget *parent, LocalModPath *modPath);
    ~LocalModBrowser();

    void load() override;

    bool isLoading() const;
    LocalModPath *modPath() const;
    QWidget *infoWidget() const override;
    QWidget *fileListWidget() const override;

    QList<QAction *> modActions() const override;
    QList<QAction *> pathActions() const override;

    QIcon icon() const override;
    QString name() const override;
signals:
    void loadFinished();
    void selectedModsChanged();

    void findNewOnCurseforge(LocalModPathInfo info);
    void findNewOnModrinth(LocalModPathInfo info);
    void findNewOnOptifine(LocalModPathInfo info);
    void findNewOnReplay(LocalModPathInfo info);
public slots:
    void reload();
    void updateModList();
    void updateUi();

private slots:
    void onLoadStarted();
    void onLoadProgress(int loadedCount, int totalCount);
    void onLoadFinished();

    void onLinkStarted();
    void onLinkProgress(int linkedCount, int totalCount);
    void onLinkFinished();

    void onCheckUpdatesStarted();
    void onCheckCancelled();
    void onUpdateCheckedCountUpdated(int checkedCount, int totalCount);
    void onUpdatesReady(int failedCount = 0);
    void onUpdatableCountChanged();

    void onUpdatesStarted();
    void onUpdatesProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onUpdatesDoneCountUpdated(int doneCount, int totalCount);
    void onUpdatesDone(int successCount, int failCount);

    void filterList();
    void updateStatusText();
    void updateProgressBar();
    void updateListViewIndexWidget();
    void updateTreeViewIndexWidget();

    void on_hideUpdatesButton_clicked();
    void onItemDoubleClicked(const QModelIndex &index);
    void on_modListView_customContextMenuRequested(const QPoint &pos);
    void on_modIconListView_customContextMenuRequested(const QPoint &pos);
    void on_modTreeView_customContextMenuRequested(const QPoint &pos);
    void onModTreeViewHeaderCustomContextMenuRequested(const QPoint &pos);
    void saveSections();
    void on_actionOpen_Folder_triggered();
    void on_actionBatch_Rename_triggered();
    void on_actionDelete_Old_Files_In_Path_triggered();
    void on_actionReload_Mods_triggered();
    void on_actionToggle_Enable_triggered(bool checked);
    void on_actionToggle_Star_triggered(bool checked);
    void on_actionRename_Selected_Mods_triggered();
    void updateSelectedMods();
    void onSelectedModsChanged();
    void on_actionOpen_Curseforge_Mod_Dialog_triggered();
    void on_actionOpen_Modrinth_Mod_Dialog_triggered();
    void on_actionOpen_Mod_Dialog_triggered();
    void on_actionSearch_on_Curseforge_triggered();
    void on_actionSearch_on_Modrinth_triggered();
    void on_actionCheck_Updates_triggered();
    void on_actionUpdate_All_triggered();
    void on_actionLink_Mod_Files_triggered();

    void on_actionExport_manifest_json_triggered();

    void on_actionExport_modlist_html_Cuseforge_triggered();

    void on_actionExport_modlist_html_Modrinth_triggered();

protected:
    void paintEvent(QPaintEvent *event) override;
private:
    Ui::LocalModBrowser *ui;
    QStandardItemModel *model_;
    QMenu *modMenu_;
    QMenu *pathMenu_;
    LocalModInfoWidget *infoWidget_;
    LocalFileListWidget *fileListWidget_;
    LocalStatusBarWidget *statusBarWidget_;
    QProgressBar *progressBar_;
    QButtonGroup *viewSwitcher_;
    LocalModPath *modPath_;
    LocalModFilter *filter_;
    QList<LocalMod*> selectedMods_;
    int hiddenCount_ = 0;

    QAbstractItemView *currentView();
    QList<LocalMod *> selectedMods(QAbstractItemView *view = nullptr);
    QMenu *getMenu(QList<LocalMod *> mods);
};

#endif // LOCALMODBROWSER_H
