#ifndef PAGESWITCHER_H
#define PAGESWITCHER_H

#include <QMdiArea>
#include <QStandardItemModel>

#include "github/githubrepoinfo.h"

class QMenuBar;
class QMainWindow;
class Browser;
class ExploreBrowser;
class DownloadBrowser;
class CurseforgeModBrowser;
class ModrinthModBrowser;
class OptifineModBrowser;
class ReplayModBrowser;
class GitHubRepoBrowser;
class LocalModBrowser;
class LocalModPath;

class PageSwitcher : public QMdiArea
{
    Q_OBJECT
public:
    enum BrowserCategory{ Download, Explore, Local };
    explicit PageSwitcher(QWidget *parent = nullptr);

    void nextPage();
    void previesPage();

    void addDownloadPage();
    void addCurseforgePage();
    void addModrinthPage();
    void addOptiFinePage();
    void addReplayModPage();
    void addGitHubPage(const GitHubRepoInfo &info);
    void addLocalPage(LocalModBrowser *browser);
    void addLocalPage(LocalModPath *path);

    void removeExplorePage(int index);
    void removeCurseforgePage();
    void removeModrinthPage();
    void removeOptiFinePage();
    void removeReplayModPage();

    LocalModBrowser *takeLocalModBrowser(int index);
    void removeLocalModBrowser(int index);

    DownloadBrowser *downloadBrowser() const;
    CurseforgeModBrowser *curseforgeModBrowser() const;
    ModrinthModBrowser *modrinthModBrowser() const;
    OptifineModBrowser *optifineModBrowser() const;
    ReplayModBrowser *replayModBrowser() const;
    const QList<ExploreBrowser *> &exploreBrowsers() const;
    const QList<LocalModBrowser *> &localModBrowsers() const;
    ExploreBrowser *exploreBrowser(int index) const;
    LocalModBrowser *localModBrowser(int index) const;
    QPair<int, int> currentCategoryPage() const;
    int currentCategory() const;
    int currentPage() const;
    Browser *currentBrowser() const;
    QStandardItemModel *model();

signals:
    void pageChanged(QModelIndex modelIndex);
    void browserChanged(Browser *previous, Browser *current);

public slots:
    void setPage(int category, int page);
    void updateUi();

private:
    void addWidget(QMainWindow *browser, int category);
    void removeExplorePage(ExploreBrowser *exploreBrowser);
    QMenuBar *menubar_;
    QVector<QList<QMdiSubWindow *>> windows_;
    QStandardItemModel model_;
    QVector<int> pageCount_;

    DownloadBrowser *downloadBrowser_ = nullptr;
    CurseforgeModBrowser *curseforgeModBrowser_ = nullptr;
    ModrinthModBrowser *modrinthModBrowser_ = nullptr;
    OptifineModBrowser *optifineModBrowser_ = nullptr;
    ReplayModBrowser *replayModBrowser_ = nullptr;
    QList<GitHubRepoBrowser *> githubBrowsers_;
    QList<ExploreBrowser *> exploreBrowsers_;
    QList<LocalModBrowser *> localModBrowsers_;
};

#endif // PAGESWITCHER_H
