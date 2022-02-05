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
    enum Column { ModColumn, NameColumn, TagNameColumn, PreReleaseColumn, CreateColumn, PublishColumn, ReleaseLinkColumn, BodyColumn };
    GitHubManagerModel(GitHubManager *manager);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setItemHeight(int newItemHeight);

private:
    GitHubManager *manager_;
    int itemHeight_ = 100;
};

class GitHubManager : public ExploreManager
{
public:
    GitHubManager(QObject *parent, const GitHubRepoInfo &info);

    void search();
    void searchMore();

    GitHubManagerModel *model() const override;
    const QList<GitHubRelease *> &releases() const;

    const GitHubRepoInfo &info() const;

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
