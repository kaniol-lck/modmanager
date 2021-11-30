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
class QAbstractItemView;
namespace Ui {
class LocalModBrowser;
}
class LocalModBrowser : public Browser
{
    Q_OBJECT
public:
    enum { ModColumn, NameColumn, IdColumn, VersionColumn, EnableColumn, StarColumn, TagsColumn, FileDateColumn, FileSizeColumn, FileNameColumn, CurseforgeIdColumn, CurseforgeFileIdColumn, ModrinthIdColumn, ModrinthFileIdColumn, DescriptionColumn };
    explicit LocalModBrowser(QWidget *parent, LocalModPath *modPath);
    ~LocalModBrowser();

    bool isLoading() const;
    LocalModPath *modPath() const;
    QWidget *infoWidget() const override;
    QWidget *fileListWidget() const override;

    QList<QAction *> modActions() const override;
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

    void onCheckWebsitesStarted();
    void onWebsiteCheckedCountUpdated(int checkedCount);
    void onWebsitesReady();

    void onCheckUpdatesStarted();
    void onCheckCancelled();
    void onUpdateCheckedCountUpdated(int updateCount, int checkedCount, int totalCount);
    void onUpdatesReady(int failedCount = 0);
    void onUpdatableCountChanged();

    void onUpdatesStarted();
    void onUpdatesProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onUpdatesDoneCountUpdated(int doneCount, int totalCount);
    void onUpdatesDone(int successCount, int failCount);

    void filterList();
    void updateStatusText();
    void updateProgressBar();
    void updateIndexWidget();

    void on_checkUpdatesButton_clicked();
    void on_updateAllButton_clicked();
    void on_hideUpdatesButton_clicked();
    void onItemSelected(const QModelIndex &index);
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
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    Ui::LocalModBrowser *ui;
    QStandardItemModel *model_;
    LocalModInfoWidget *infoWidget_;
    LocalFileListWidget *fileListWidget_;
    QStatusBar *statusBar_;
    QProgressBar *progressBar_;
    QButtonGroup *viewSwitcher_;
    LocalModPath *modPath_;
    LocalModFilter *filter_;
    QList<LocalMod*> selectedMods_;
    int hiddenCount_ = 0;
    bool isChecking_ = false;
    bool isUpdating_ = false;

    QAbstractItemView *currentView();
    QList<LocalMod *> selectedMods(QAbstractItemView *view = nullptr);
    QMenu *getMenu(QList<LocalMod *> mods);
};

#endif // LOCALMODBROWSER_H
