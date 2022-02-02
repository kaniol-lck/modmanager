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
class CurseforgeManager;
class LocalMod;
class QMenu;

class CurseforgeManagerProxyModel;
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

    ExploreBrowser *another() override;

signals:
    void selectedModsChanged(CurseforgeMod *selectedMod);

public slots:
    void refresh() override;
    void searchModByPathInfo(LocalModPath *path) override;
    void updateUi() override;

private slots:
    void switchSection();
    void updateVersionList();
    void updateCategoryList(QList<CurseforgeCategoryInfo> list);
    void search();
    void on_loaderSelect_currentIndexChanged(int index);
    void on_sortSelect_currentIndexChanged(int);
    void on_actionMod_triggered();
    void on_actionWorld_triggered();
    void on_actionModpacks_triggered();
    void on_actionTexturepacks_triggered();
    void on_menuDownload_aboutToShow();
    void on_actionCopy_Website_Link_triggered();
    void on_actionOpen_Curseforge_Mod_Dialog_triggered();
    void on_actionOpen_Website_Link_triggered();

private:
    Ui::CurseforgeModBrowser *ui;
    CurseforgeManager *manager_;
    CurseforgeManagerProxyModel *proxyModel_;
    CurseforgeAPI::Section sectionId_;
    CurseforgeModInfoWidget *infoWidget_;
    CurseforgeFileListWidget *fileListWidget_;
    QList<int> idList_;
    //open as a dialog to search mod
    LocalMod *localMod_;
    ModLoaderType::Type currentLoaderType_ = ModLoaderType::Any;
    bool inited_ = false;
    CurseforgeMod* selectedMod_ = nullptr;
    std::unique_ptr<Reply<QList<CurseforgeCategoryInfo> > > sectionCategoriesGetter_;

    int currentCategoryId_ = 0;
    GameVersion currentGameVersion_;

    void loadMore() override;
    void onSelectedItemChanged(const QModelIndex &index) override;
    QDialog *getDialog(const QModelIndex &index) override;
    QWidget *getIndexWidget(const QModelIndex &index) override;
    QMenu *getCustomContextMenu() override;
};

#endif // CURSEFORGEMODBROWSER_H
