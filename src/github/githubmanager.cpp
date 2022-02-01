#include "githubmanager.h"

#include "githubrelease.h"
#include "config.hpp"

const QList<GitHubRelease *> &GitHubManager::releases() const
{
    return releases_;
}

void GitHubManager::getModList()
{
    emit searchStarted();
    searchModsGetter_ = api_.getReleases(info_, currentPage_).asUnique();
    searchModsGetter_->setOnFinished(this, [=](const auto &infoList){
        //new search
        if(currentPage_ == 1){
            emit scrollToTop();
            model_->beginResetModel();
            for(auto &&mod : releases_)
                if(mod->parent() == this) mod->deleteLater();
            releases_.clear();
            model_->endResetModel();
            hasMore_ = true;
        }
        model_->beginInsertRows({}, releases_.size(), releases_.size() + infoList.size());
        for(auto releaseInfo : infoList){
            auto release = new GitHubRelease(this, releaseInfo);
            releases_ << release;
        }
        model_->endInsertRows();
        hasMore_ = infoList.size() == Config().getSearchResultCount();
        emit searchFinished();
    }, [=](auto){
        emit searchFinished(false);
    });
}

GitHubManager::GitHubManager(QObject *parent, const GitHubRepoInfo &info) :
    ExploreManager(parent),
    info_(info),
    model_(new GitHubManagerModel(this))
{}

void GitHubManager::search()
{
    currentPage_ = 1;
    getModList();
}

void GitHubManager::searchMore()
{
    if(!hasMore_) return;
    currentPage_++;
    getModList();
}

GitHubManagerModel *GitHubManager::model() const
{
    return model_;
}

GitHubManagerModel::GitHubManagerModel(GitHubManager *manager) :
    QAbstractListModel(manager),
    manager_(manager)
{}

int GitHubManagerModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return manager_->releases().size();
}

QVariant GitHubManagerModel::data(const QModelIndex &index, int role) const
{
    //    qDebug() << index << role;
        if (!index.isValid() || index.row() >= rowCount())
            return QVariant();
        auto mod = manager_->releases().at(index.row());
        switch (role) {
        case Qt::UserRole + 1:
            return QVariant::fromValue(mod);
            break;
        case Qt::SizeHintRole:
            return QSize(0, itemHeight_);
            break;
        }
        return QVariant();
}

void GitHubManagerModel::setItemHeight(int newItemHeight)
{
    itemHeight_ = newItemHeight;
}
