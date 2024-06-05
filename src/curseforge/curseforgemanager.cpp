#include "curseforgemanager.h"

#include "curseforgemod.h"
#include "config.hpp"

CurseforgeManager::CurseforgeManager(QObject *parent, CurseforgeAPI::Section sectionId) :
    ExploreManager(parent),
    model_(new CurseforgeManagerModel(this)),
    sectionId_(sectionId)
{}

void CurseforgeManager::search(const QString &name, int categoryId, GameVersion gameVersion, int sort, bool sortOrderAsc)
{
    currentIndex_ = 0;
    currentName_ = name;
    currentCategoryId_ = categoryId;
    currentGameVersion_ = gameVersion;
    currentSort_ = sort;
    currentSortOrderAsc_ = sortOrderAsc;
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
    searchModsGetter_ = api_.searchMods(sectionId_, currentGameVersion_, currentLoaderType_, currentIndex_, currentName_, currentCategoryId_, currentSort_, currentSortOrderAsc_).asUnique();
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
        case SlugColumn:
            return tr("Slug");
        case AuthorsColumn:
            return tr("Authors");
        case CategoryColumn:
            return tr("Categories");
        case WebsiteColumn:
            return tr("Website Link");
        case UpdateDateColumn:
            return tr("Date Modified");
        case CreateDateColumn:
            return tr("Date Created");
        case ReleaseDateColumn:
            return tr("Date Released");
        case DownloadCountColumn:
            return tr("Download Count");
        case PopularityRankColumn:
            return tr("Popularity Rank");
        case SummaryColumn:
            return tr("Summary");
            break;
        }
        break;
    case Qt::DecorationRole:
        switch (section)
        {
        case CategoryColumn:
            return QIcon::fromTheme("tag");
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
            case SlugColumn:
                return mod->modInfo().slug();
            case AuthorsColumn:
                return mod->modInfo().authors().join(", ");
            case CategoryColumn:
                break;
            case WebsiteColumn:
                return mod->modInfo().websiteUrl();
            case UpdateDateColumn:
                return mod->modInfo().dateModified().toString(QLocale().dateTimeFormat());
            case CreateDateColumn:
                return mod->modInfo().dateCreated().toString(QLocale().dateTimeFormat());
            case ReleaseDateColumn:
                return mod->modInfo().dateReleased().toString(QLocale().dateTimeFormat());
            case DownloadCountColumn:
                return mod->modInfo().downloadCount();
            case PopularityRankColumn:
                return mod->modInfo().gamePopularityRank();
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
