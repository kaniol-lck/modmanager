#include "modrinthmanager.h"

#include "modrinthmod.h"
#include "config.hpp"

ModrinthManager::ModrinthManager(QObject *parent) :
    ExploreManager(parent),
    model_(new ModrinthManagerModel(this))
{}

void ModrinthManager::search(const QString name, const QList<GameVersion> &versions, ModLoaderType::Type type, const QList<QString> &categories, int sort)
{
    currentIndex_ = 0;
    currentName_ = name;
    currentGameVersions_ = versions;
    currentType_ = type;
    currentCategoryIds_ = categories;
    currentSort_ = sort;
    getModList();
}

void ModrinthManager::searchMore()
{
    if(!hasMore_) return;
    currentIndex_ += Config().getSearchResultCount();
    getModList();
}

void ModrinthManager::refresh()
{
    getModList();
}

ModrinthManagerModel *ModrinthManager::model() const
{
    return model_;
}

const QList<ModrinthMod *> &ModrinthManager::mods() const
{
    return mods_;
}

void ModrinthManager::getModList()
{
    emit searchStarted();
    searchModsGetter_ = api_.searchMods(currentName_, currentIndex_, currentGameVersions_, currentType_, currentCategoryIds_, currentSort_).asUnique();
    searchModsGetter_->setOnFinished(this, [=](const QList<ModrinthModInfo> &infoList){
        //new search
        if(currentIndex_ == 0){
            emit scrollToTop();
            model_->beginResetModel();
            for(auto &&mod : mods_)
                if(mod->parent() == this) mod->deleteLater();
            mods_.clear();
            model_->endResetModel();
            hasMore_ = true;
        }

        model_->beginInsertRows({}, mods_.size(), mods_.size() + infoList.size());
        for(const auto &info : qAsConst(infoList)){
            auto mod = new ModrinthMod(this, info);
            mod->acquireIcon();
            mods_ << mod;
        }
        model_->endInsertRows();
        hasMore_ = infoList.size() == Config().getSearchResultCount();
        emit searchFinished();
    }, [=](auto){
        emit searchFinished(false);
    });
}

ModrinthManagerModel::ModrinthManagerModel(ModrinthManager *manager) :
    QAbstractListModel(manager),
    manager_(manager)
{}

int ModrinthManagerModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return manager_->mods().size();
}

QVariant ModrinthManagerModel::data(const QModelIndex &index, int role) const
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

void ModrinthManagerModel::setItemHeight(int newItemHeight)
{
    itemHeight_ = newItemHeight;
}
