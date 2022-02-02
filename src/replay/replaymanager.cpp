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

ReplayManagerProxyModel::ReplayManagerProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{}

void ReplayManagerProxyModel::setGameVersion(const GameVersion &newGameVersion)
{
    gameVersion_ = newGameVersion;
}

void ReplayManagerProxyModel::setLoaderType(ModLoaderType::Type newLoaderType)
{
    loaderType_ = newLoaderType;
}

void ReplayManagerProxyModel::setText(const QString &newText)
{
    text_ = newText;
}

bool ReplayManagerProxyModel::filterAcceptsRow(int source_row, const QModelIndex &) const
{
    auto mod = sourceModel()->index(source_row, ReplayManager::ModColumn).data(Qt::UserRole + 1).value<ReplayMod *>();
    if(gameVersion_ != GameVersion::Any && gameVersion_ != mod->modInfo().gameVersion()) return false;
    if(auto loaderType = mod->modInfo().loaderType(); loaderType_ != ModLoaderType::Any && loaderType !=ModLoaderType::Any &&
            loaderType_ != loaderType) return false;
    if(!mod->modInfo().fileName().contains(text_)) return false;
    return true;
}
