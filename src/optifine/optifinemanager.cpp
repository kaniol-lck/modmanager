#include "optifinemanager.h"

#include "optifinemod.h"
#include "config.hpp"

#include <QSize>

OptifineManager::OptifineManager(QObject *parent) :
    ExploreManager(parent),
    model_(new OptifineManagerModel(this))
{}

void OptifineManager::search()
{
    getModList();
}

const QList<OptifineMod *> &OptifineManager::mods() const
{
    return mods_;
}

OptifineManagerModel *OptifineManager::model() const
{
    return model_;
}

void OptifineManager::getModList()
{
    emit searchStarted();
    auto source = Config().getOptifineSource();
    if(source == Config::OptifineSourceType::Official)
        searchModsGetter_ = api_.getModList().asUnique();
    else if (source == Config::OptifineSourceType::BMCLAPI)
        searchModsGetter_ = bmclapi_.getOptifineList().asUnique();
    searchModsGetter_->setOnFinished(this, [=](const auto &list){
        model_->beginResetModel();
        for(auto &&mod : mods_)
            if(mod->parent() == this) mod->deleteLater();
        mods_.clear();
        for(auto &&modInfo : list){
            auto mod = new OptifineMod(nullptr, modInfo);
            mods_ << mod;
        }
        model_->endResetModel();
        emit searchFinished();
    }, [=](auto){
        emit searchFinished(false);
    });
}

OptifineManagerModel::OptifineManagerModel(OptifineManager *manager) :
    QAbstractListModel(manager),
    manager_(manager)
{}

int OptifineManagerModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return manager_->mods().size();
}

QVariant OptifineManagerModel::data(const QModelIndex &index, int role) const
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

void OptifineManagerModel::setItemHeight(int newItemHeight)
{
    itemHeight_ = newItemHeight;
}
