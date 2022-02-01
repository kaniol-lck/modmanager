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

class GitHubManager;
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
    void updateStatusText() override;

private:
    GitHubRepoInfo info_;
    Ui::GitHubRepoBrowser *ui;
    GitHubManager *manager_;
    GitHubReleaseInfoWidget *infoWidget_;
    GitHubFileListWidget *fileListWidget_;
    bool inited_ = false;
    GitHubRelease* selectedRelease_ = nullptr;

    void loadMore() override;
    void onSelectedItemChanged(const QModelIndex &index) override;
    QWidget *getIndexWidget(const QModelIndex &index) override;
};

#endif // GITHUBREPOBROWSER_H
