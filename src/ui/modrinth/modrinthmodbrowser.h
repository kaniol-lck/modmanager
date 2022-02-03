#ifndef MODRINTHMODBROWSER_H
#define MODRINTHMODBROWSER_H

#include "ui/explorebrowser.h"

class ModrinthAPI;
class ModrinthMod;
class ModrinthManager;
class LocalModPath;
class ModrinthModInfoWidget;
class ModrinthFileListWidget;
class ExploreStatusBarWidget;
class ExploreStatusBarWidget;
class QStandardItemModel;
class LocalMod;

class ModrinthModInfo;
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

    ExploreBrowser *another() override;

signals:
    void selectedModsChanged(ModrinthMod *selectedMod);

public slots:
    void refresh() override;
    void searchModByPathInfo(LocalModPath *path) override;
    void updateUi() override;

private slots:
    void updateVersionList();
    void updateCategoryList();
    void search();
    void on_sortSelect_currentIndexChanged(int);
    void on_loaderSelect_currentIndexChanged(int);
    void on_menuDownload_aboutToShow();
    void on_actionCopy_Website_Link_triggered();
    void on_actionOpen_Modrinth_Mod_Dialog_triggered();
    void on_actionOpen_Website_Link_triggered();

private:
    Ui::ModrinthModBrowser *ui;
    ModrinthManager *manager_;
    ModrinthModInfoWidget *infoWidget_;
    ModrinthFileListWidget *fileListWidget_;
    QStringList lastCategoryIds_;
    QStringList currentCategoryIds_;
    QList<GameVersion> lastGameVersions_;
    QList<GameVersion> currentGameVersions_;
    bool inited_ = false;
    LocalMod *localMod_ = nullptr;
    ModrinthMod* selectedMod_ = nullptr;

    void loadMore() override;
    void onSelectedItemChanged(const QModelIndex &index) override;
    QDialog *getDialog(const QModelIndex &index) override;
    QWidget *getListViewIndexWidget(const QModelIndex &index) override;
    QWidget *getTreeViewIndexWidget(const QModelIndex &index) override;
    QMenu *getCustomContextMenu() override;
};

#endif // MODRINTHMODBROWSER_H
