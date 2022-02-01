#ifndef GITHUBMANAGER_H
#define GITHUBMANAGER_H

#include "exploremanager.h"
#include "githubapi.h"

class GitHubRelease;
class GitHubManager;
class GitHubManagerModel : public QAbstractListModel
{
    Q_OBJECT
    friend class GitHubManager;
public:
    GitHubManagerModel(GitHubManager *manager);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setItemHeight(int newItemHeight);

private:
    GitHubManager *manager_;
    int itemHeight_ = 100;
};

class GitHubManager : public ExploreManager
{
public:
    enum Column { ModColumn };

    GitHubManager(QObject *parent, const GitHubRepoInfo &info);

    void search();
    void searchMore();

    GitHubManagerModel *model() const override;
    const QList<GitHubRelease *> &releases() const;

private:
    GitHubRepoInfo info_;
    GitHubAPI api_;
    GitHubManagerModel *model_;
    QList<GitHubRelease *> releases_;
    int currentPage_ = 1;
    bool hasMore_ = false;
    std::unique_ptr<Reply<QList<GitHubReleaseInfo> > > searchModsGetter_;

    void getModList() override;
};

#endif // GITHUBMANAGER_H
