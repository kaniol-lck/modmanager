#include "githubmanager.h"

#include "githubrelease.h"
#include "config.hpp"

const QList<GitHubRelease *> &GitHubManager::releases() const
{
    return releases_;
}

const GitHubRepoInfo &GitHubManager::info() const
{
    return info_;
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

QVariant GitHubManagerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation != Qt::Horizontal) return QVariant();
    switch (role) {
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
        switch (section)
        {
        case ModColumn:
            return QVariant();
        case NameColumn:
            return tr("Name");
        case TagNameColumn:
            return tr("Tag Name");
        case PreReleaseColumn:
            return tr("Pre Release");
        case CreateColumn:
            return tr("Date Created");
        case PublishColumn:
            return tr("Date Published");
        case ReleaseLinkColumn:
            return tr("Release Link");
        case BodyColumn:
            return tr("Body");
        }
        break;
    case Qt::DecorationRole:
        switch (section)
        {
        }
        break;
    }
    return QVariant();
}

int GitHubManagerModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return manager_->releases().size();
}

int GitHubManagerModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return BodyColumn + 1;
}

QVariant GitHubManagerModel::data(const QModelIndex &index, int role) const
{
    //    qDebug() << index << role;
        if (!index.isValid() || index.row() >= rowCount())
            return QVariant();
        auto release = manager_->releases().at(index.row());
        switch (role) {
        case Qt::CheckStateRole:
            switch (index.column()) {
            case PreReleaseColumn:
                return release->info().prerelease()? Qt::Checked : Qt::Unchecked;
            }
            break;
        case Qt::ToolTipRole:
        case Qt::DisplayRole:
            switch (index.column())
            {
            case ModColumn:
                break;
            case NameColumn:
                return release->info().name();
            case TagNameColumn:
                return release->info().tagName();
            case CreateColumn:
                return release->info().created().toString(QLocale().dateTimeFormat());
            case PublishColumn:
                return release->info().published().toString(QLocale().dateTimeFormat());
            case ReleaseLinkColumn:
                return release->info().url();
            case BodyColumn:
                return release->info().body();
            }
            break;
        case Qt::DecorationRole:
            switch (index.column())
            {
            case NameColumn:
                return manager_->info().icon();
            }
            break;
        case Qt::UserRole + 1:
            switch (index.column())
            {
            case ModColumn:
                return QVariant::fromValue(release);
            }
            break;
        case Qt::SizeHintRole:
            if(index.column() == ModColumn){
                if(auto height = itemHeights_[index])
                    return QSize(0, height);
                else
                    return QSize(0, 200);
            }
            break;
        }
        return QVariant();
}

void GitHubManagerModel::setItemHeight(const QModelIndex &index, int newItemHeight)
{
    itemHeights_[index] = newItemHeight;
}
