#ifndef GITHUBREPOBROWSER_H
#define GITHUBREPOBROWSER_H

#include "ui/explorebrowser.h"

#include "network/reply.hpp"
#include "github/githubrepoinfo.h"
#include "github/githubreleaseinfo.h"

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
    void searchModByPathInfo(LocalModPath *path) override;
    void updateUi() override;

    ExploreBrowser *another() override;

signals:
    void downloadPathChanged(LocalModPath *path);

private slots:
//    void filterList();
    void updateStatusText();

private:
    GitHubRepoInfo info_;
    Ui::GitHubRepoBrowser *ui;
    QStandardItemModel *model_;
    GitHubReleaseInfoWidget *infoWidget_;
    GitHubFileListWidget *fileListWidget_;
    GitHubAPI *api_;
    bool inited_ = false;
    GitHubRelease* selectedRelease_ = nullptr;
    int currentPage_ = 1;
    bool hasMore_ = false;
    std::unique_ptr<Reply<QList<GitHubReleaseInfo> > > searchModsGetter_;

    void getReleaseList(int page = 1);
    void loadMore() override;
    void onSelectedItemChanged(const QModelIndex &index) override;
    QWidget *getIndexWidget(const QModelIndex &index) override;
};

#endif // GITHUBREPOBROWSER_H
