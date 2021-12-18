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
    static constexpr int SubWindowRole = 433;
public:
    enum BrowserCategory{ Download, Explore, Local };
    explicit PageSwitcher(QWidget *parent = nullptr);

    void nextPage();
    void previesPage();

    void addDownloadPage();

    ExploreBrowser *exploreBrowser(int index) const;
    void addExploreBrowser(ExploreBrowser *browser);
    void removeExploreBrowser(int index);

    LocalModBrowser *localModBrowser(int index) const;
    void addLocalBrowser(LocalModBrowser *browser);
    void removeLocalBrowser(int index);

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
    void syncPathList();
    void updateUi();

private:
    void addBrowser(Browser *browser, BrowserCategory category);
    QMdiSubWindow *addSubWindowForItem(QStandardItem *item);
    void removeSubWindowForItem(QStandardItem *item);
    void removeExploreBrowser(ExploreBrowser *exploreBrowser);
    int findLocalBrowser(LocalModPath *path);
    QMenuBar *menubar_;
    QStandardItemModel model_;

    bool isSyncing_ = false;
};

#endif // PAGESWITCHER_H
