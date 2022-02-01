#include "curseforgemanager.h"

#include "curseforgemod.h"
#include "config.hpp"

CurseforgeManager::CurseforgeManager(QObject *parent, CurseforgeAPI::Section sectionId) :
    ExploreManager(parent),
    model_(new CurseforgeManagerModel(this)),
    sectionId_(sectionId)
{}

void CurseforgeManager::search(const QString &name, int categoryId, GameVersion gameVersion, int sort)
{
    currentIndex_ = 0;
    currentName_ = name;
    currentCategoryId_ = categoryId;
    currentGameVersion_ = gameVersion;
    currentSort_ = sort;
    getModList();
}

void CurseforgeManager::searchMore()
{
    if(!hasMore_) return;
    currentIndex_ += Config().getSearchResultCount();
    getModList();
}

void CurseforgeManager::refresh()
{
    getModList();
}

CurseforgeManagerModel *CurseforgeManager::model() const
{
    return model_;
}

const QList<CurseforgeMod *> &CurseforgeManager::mods() const
{
    return mods_;
}

void CurseforgeManager::setSectionId(CurseforgeAPI::Section newSectionId)
{
    sectionId_ = newSectionId;
}

void CurseforgeManager::getModList()
{
    emit searchStarted();
    searchModsGetter_ = api_.searchMods(sectionId_, currentGameVersion_, currentIndex_, currentName_, currentCategoryId_, currentSort_).asUnique();
    searchModsGetter_->setOnFinished(this, [=](const QList<CurseforgeModInfo> &infoList){
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

        //show them
        model_->beginInsertRows({}, mods_.size(), mods_.size() + infoList.size());
        qDebug() << "info list size" <<  infoList.size();
        for(const auto &info : qAsConst(infoList)){
            //do not load duplicate mod
//            if(std::find_if(mods_.cbegin(), mods_.cend(), [&](auto &&mod){
//                            return mod.id() == info.id();
//        }) == mods_.cend()) continue;
            auto mod = new CurseforgeMod(this, info);
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

CurseforgeManagerModel::CurseforgeManagerModel(CurseforgeManager *manager) :
    QAbstractListModel(manager),
    manager_(manager)
{}

int CurseforgeManagerModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return manager_->mods().size();
}

QVariant CurseforgeManagerModel::data(const QModelIndex &index, int role) const
{
    //    qDebug() << index << role;
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

void CurseforgeManagerModel::setItemHeight(int newItemHeight)
{
    itemHeight_ = newItemHeight;
}

CurseforgeManagerProxyModel::CurseforgeManagerProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{}

void CurseforgeManagerProxyModel::setLoaderType(ModLoaderType::Type newLoaderType)
{
    loaderType_ = newLoaderType;
}

bool CurseforgeManagerProxyModel::filterAcceptsRow(int source_row, const QModelIndex &) const
{
    if(loaderType_ == ModLoaderType::Any) return true;
    auto mod = sourceModel()->index(source_row, CurseforgeManager::ModColumn).data(Qt::UserRole + 1).value<CurseforgeMod *>();
    if(!mod) return false;
    return mod->modInfo().loaderTypes().contains(loaderType_) || mod->modInfo().loaderTypes().isEmpty();
}
