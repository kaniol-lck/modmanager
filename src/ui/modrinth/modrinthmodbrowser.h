#ifndef MODRINTHMODBROWSER_H
#define MODRINTHMODBROWSER_H

#include "ui/explorebrowser.h"

class ModrinthAPI;
class ModrinthMod;
class LocalModPath;
class ModrinthModInfoWidget;
class ModrinthFileListWidget;
class ExploreStatusBarWidget;
class ExploreStatusBarWidget;
class QStandardItemModel;
class LocalMod;
namespace Ui {
class ModrinthModBrowser;
}

class ModrinthModBrowser : public ExploreBrowser
{
    Q_OBJECT

public:
    explicit ModrinthModBrowser(QWidget *parent = nullptr, LocalMod *localMod = nullptr);
    ~ModrinthModBrowser();

    void load() override;

    QWidget *infoWidget() const override;
    QWidget *fileListWidget() const override;
    ModrinthMod *selectedMod() const;

    QList<QAction *> modActions() const override;

    ExploreBrowser *another() override;

signals:
    void downloadPathChanged(LocalModPath *path);
    void selectedModsChanged(ModrinthMod *selectedMod);

public slots:
    void refresh() override;
    void searchModByPathInfo(const LocalModPathInfo &info) override;
    void updateUi() override;

private slots:
    void updateVersionList();
    void updateCategoryList();
    void updateLocalPathList();
    void search();
    void onSliderChanged(int i);
    void updateStatusText();
    void on_modListView_doubleClicked(const QModelIndex &index);
    void on_sortSelect_currentIndexChanged(int);
    void on_loaderSelect_currentIndexChanged(int);
    void on_downloadPathSelect_currentIndexChanged(int index);
    void onItemSelected();
    void updateIndexWidget();
    void on_actionOpen_Folder_triggered();
    void on_menuDownload_aboutToShow();
    void on_actionCopy_Website_Link_triggered();
    void on_actionOpen_Modrinth_Mod_Dialog_triggered();
    void on_modListView_customContextMenuRequested(const QPoint &pos);
    void on_actionOpen_Website_Link_triggered();

protected:
    void paintEvent(QPaintEvent *event) override;
private:
    Ui::ModrinthModBrowser *ui;
    QStandardItemModel *model_;
    ModrinthModInfoWidget *infoWidget_;
    ModrinthFileListWidget *fileListWidget_;
    ExploreStatusBarWidget *statusBarWidget_;
    ModrinthAPI *api_;
    LocalModPath *downloadPath_ = nullptr;
    QString currentName_;
    int currentIndex_;
    QStringList lastCategoryIds_;
    QStringList currentCategoryIds_;
    QList<GameVersion> lastGameVersions_;
    QList<GameVersion> currentGameVersions_;
    bool hasMore_ = false;
    bool isSearching_ = false;
    bool inited_ = false;
    LocalMod *localMod_ = nullptr;
    ModrinthMod* selectedMod_ = nullptr;

    void getModList(QString name, int index = 0);
};

#endif // MODRINTHMODBROWSER_H
