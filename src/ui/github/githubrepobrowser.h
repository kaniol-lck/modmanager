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
    void onSliderChanged(int i);
//    void updateLocalPathList();
//    void filterList();
    void updateStatusText();
    void updateIndexWidget();
//    void on_actionOpen_Folder_triggered();
    void onItemSelected();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    GitHubRepoInfo info_;
    Ui::GitHubRepoBrowser *ui;
    QStandardItemModel *model_;
    GitHubReleaseInfoWidget *infoWidget_;
    GitHubFileListWidget *fileListWidget_;
    ExploreStatusBarWidget *statusBarWidget_;
    GitHubAPI *api_;
    LocalModPath *downloadPath_ = nullptr;
    bool inited_ = false;
    GitHubRelease* selectedRelease_ = nullptr;
    int currentPage_ = 1;
    bool hasMore_ = false;
    bool isSearching_ = false;

    void getReleaseList(int page = 1);
};

#endif // GITHUBREPOBROWSER_H
