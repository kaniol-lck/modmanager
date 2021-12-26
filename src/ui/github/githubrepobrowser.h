#ifndef GITHUBREPOBROWSER_H
#define GITHUBREPOBROWSER_H

#include "ui/explorebrowser.h"

#include "github/githubrepoinfo.h"

class GitHubFileListWidget;
class ExploreStatusBarWidget;
class LocalModPath;
class QStandardItemModel;
class GitHubAPI;
class GitHubReleaseInfoWidget;
class GitHubRelease;
namespace Ui {
class GitHubRepoBrowser;
}

class GitHubRepoBrowser : public ExploreBrowser
{
    Q_OBJECT

public:
    explicit GitHubRepoBrowser(QWidget *parent, const GitHubRepoInfo &info);
    ~GitHubRepoBrowser();

    void load() override;

    QWidget *infoWidget() const override;
    QWidget *fileListWidget() const override;

public slots:
    void refresh() override;
    void searchModByPathInfo(const LocalModPathInfo &info) override;
    void updateUi() override;

    ExploreBrowser *another() override;

signals:
    void downloadPathChanged(LocalModPath *path);

private slots:
//    void updateLocalPathList();
//    void filterList();
    void updateStatusText();
//    void on_actionOpen_Folder_triggered();

private:
    GitHubRepoInfo info_;
    Ui::GitHubRepoBrowser *ui;
    GitHubReleaseInfoWidget *infoWidget_;
    GitHubFileListWidget *fileListWidget_;
    GitHubAPI *api_;
    bool inited_ = false;
    GitHubRelease* selectedRelease_ = nullptr;
    int currentPage_ = 1;
    bool hasMore_ = false;
    bool isSearching_ = false;

    void getReleaseList(int page = 1);
    void loadMore() override;
    void onSelectedItemChanged(QStandardItem *item) override;
    QWidget *getIndexWidget(QStandardItem *item) override;
};

#endif // GITHUBREPOBROWSER_H
