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
    QAbstractTableModel(manager),
    manager_(manager)
{}

QVariant CurseforgeManagerModel::headerData(int section, Qt::Orientation orientation, int role) const
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
        case ProjectIDColumn:
            return tr("Project ID");
        case WebsiteColumn:
            return tr("Website Link");
        case UpdateDateColumn:
            return tr("Date Modified");
        case CreateDateColumn:
            return tr("Date Created");
        case ReleaseDateColumn:
            return tr("Date Released");
        case SummaryColumn:
            return tr("Summary");
            break;
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

int CurseforgeManagerModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return manager_->mods().size();
}

int CurseforgeManagerModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return SummaryColumn + 1;
}

QVariant CurseforgeManagerModel::data(const QModelIndex &index, int role) const
{
    //    qDebug() << index << role;
        if (!index.isValid() || index.row() >= rowCount())
            return QVariant();
        auto mod = manager_->mods().at(index.row());
        switch (role) {
        case Qt::ToolTipRole:
        case Qt::DisplayRole:
            switch (index.column())
            {
            case ModColumn:
                break;
            case NameColumn:
                return mod->modInfo().name();
            case ProjectIDColumn:
                return mod->modInfo().id();
            case WebsiteColumn:
                return mod->modInfo().websiteUrl();
            case UpdateDateColumn:
                return mod->modInfo().dateModified().toString(QLocale().dateTimeFormat());
            case CreateDateColumn:
                return mod->modInfo().dateCreated().toString(QLocale().dateTimeFormat());
            case ReleaseDateColumn:
                return mod->modInfo().dateReleased().toString(QLocale().dateTimeFormat());
            case SummaryColumn:
                return mod->modInfo().summary();
                break;
            }
            break;
        case Qt::DecorationRole:
            switch (index.column())
            {
            case NameColumn:
                return QIcon(mod->modInfo().icon().scaled(96, 96, Qt::KeepAspectRatio));
            }
            break;
        case Qt::UserRole + 1:
            switch (index.column())
            {
            case ModColumn:
                return QVariant::fromValue(mod);
            }
            break;
        case Qt::SizeHintRole:
            if(index.column() == ModColumn)
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
    auto mod = sourceModel()->index(source_row, CurseforgeManagerModel::ModColumn).data(Qt::UserRole + 1).value<CurseforgeMod *>();
    if(!mod) return false;
    return mod->modInfo().loaderTypes().contains(loaderType_) || mod->modInfo().loaderTypes().isEmpty();
}
