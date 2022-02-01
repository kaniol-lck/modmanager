#include "replaymanager.h"

#include "replaymod.h"

ReplayManager::ReplayManager(QObject *parent) :
    ExploreManager(parent),
    model_(new ReplayManagerModel(this))
{}

void ReplayManager::search()
{
    getModList();
}

ReplayManagerModel *ReplayManager::model() const
{
    return model_;
}

const QList<ReplayMod *> &ReplayManager::mods() const
{
    return mods_;
}

void ReplayManager::getModList()
{
    emit searchStarted();
    searchModsGetter_ = api_.getModList().asUnique();
    searchModsGetter_->setOnFinished(this, [=](const auto &list){
        model_->beginResetModel();
        for(auto &&mod : mods_)
            if(mod->parent() == this) mod->deleteLater();
        mods_.clear();
        for(auto modInfo : list){
            auto mod = new ReplayMod(this, modInfo);
            mods_ << mod;
        }
        model_->endResetModel();
        emit searchFinished();
    }, [=](auto){
        emit searchFinished(false);
    });
}

ReplayManagerModel::ReplayManagerModel(ReplayManager *manager) :
    QAbstractListModel(manager),
    manager_(manager)
{}

int ReplayManagerModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return manager_->mods().size();
}

QVariant ReplayManagerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount())
        return QVariant();
    auto mod = manager_->mods().at(index.row());
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

void ReplayManagerModel::setItemHeight(int newItemHeight)
{
    itemHeight_ = newItemHeight;
}
