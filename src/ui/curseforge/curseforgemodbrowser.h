#ifndef CURSEFORGEMODBROWSER_H
#define CURSEFORGEMODBROWSER_H

#include "ui/explorebrowser.h"

#include "curseforge/curseforgeapi.h"

class CurseforgeMod;
class LocalModPath;
class CurseforgeModInfoWidget;
class CurseforgeFileListWidget;
class ExploreStatusBarWidget;
class QStatusBar;
class QStandardItemModel;
class LocalMod;
class QMenu;
namespace Ui {
class CurseforgeModBrowser;
}

class CurseforgeModBrowser : public ExploreBrowser
{
    Q_OBJECT

public:
    explicit CurseforgeModBrowser(QWidget *parent = nullptr, LocalMod *mod = nullptr, CurseforgeAPI::Section sectionId = CurseforgeAPI::Mod);
    ~CurseforgeModBrowser();

    void load() override;

    QWidget *infoWidget() const override;
    QWidget *fileListWidget() const override;
    CurseforgeMod *selectedMod() const;

    QList<QAction *> modActions() const override;
    QList<QAction *> pathActions() const override;

    ExploreBrowser *another() override;

signals:
    void downloadPathChanged(LocalModPath *path);
    void selectedModsChanged(CurseforgeMod *selectedMod);

public slots:
    void refresh() override;
    void searchModByPathInfo(const LocalModPathInfo &info) override;
    void updateUi() override;

private slots:
    void getModList(QString name, int index = 0, int needMore = 20);
    void updateVersionList();
    void updateCategoryList();
    void updateCategoryList(QList<CurseforgeCategoryInfo> list);
    void updateLocalPathList();
    void search();
    void onSliderChanged(int i);
    void updateStatusText();
    void on_modListView_doubleClicked(const QModelIndex &index);
    void on_loaderSelect_currentIndexChanged(int index);
    void on_downloadPathSelect_currentIndexChanged(int index);
    void onItemSelected();
    void updateIndexWidget();
    void on_sortSelect_currentIndexChanged(int);
    void on_actionOpen_Folder_triggered();
    void on_actionMod_triggered();
    void on_actionWorld_triggered();
    void on_actionModpacks_triggered();
    void on_actionTexturepacks_triggered();

protected:
    void paintEvent(QPaintEvent *event) override;
private:
    Ui::CurseforgeModBrowser *ui;
    CurseforgeAPI::Section sectionId_ = CurseforgeAPI::Mod;
    QStandardItemModel *model_;
    CurseforgeModInfoWidget *infoWidget_;
    CurseforgeFileListWidget *fileListWidget_;
    ExploreStatusBarWidget *statusBarWidget_;
    CurseforgeAPI *api_;
    QList<int> idList_;
    LocalModPath *downloadPath_ = nullptr;
    //open as a dialog to search mod
    LocalMod *localMod_;
    QString currentName_;
    int currentIndex_;
    int currentCategoryId_;
    GameVersion currentGameVersion_;
    ModLoaderType::Type currentLoaderType_ = ModLoaderType::Any;
    bool hasMore_ = false;
    bool isSearching_ = false;
    bool inited_ = false;
    CurseforgeMod* selectedMod_ = nullptr;
};

#endif // CURSEFORGEMODBROWSER_H
