#include "optifinemanager.h"

#include <QSize>

#include "optifinemod.h"
#include "config.hpp"

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
    QAbstractTableModel(manager),
    manager_(manager)
{}

QVariant OptifineManagerModel::headerData(int section, Qt::Orientation orientation, int role) const
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
        case GameVersionColumn:
            return tr("Game Version");
        case FileNameColumn:
            return tr("File Name");
        case PreviewColumn:
            return tr("Preview");
        case MirrorLinkColumn:
            return tr("Mirror Link");
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

int OptifineManagerModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return manager_->mods().size();
}

int OptifineManagerModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return MirrorLinkColumn + 1;
}

QVariant OptifineManagerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount())
        return QVariant();
    auto mod = manager_->mods().at(index.row());
    switch (role) {
    case Qt::CheckStateRole:
        switch (index.column()) {
        case PreviewColumn:
            return mod->modInfo().isPreview()? Qt::Checked : Qt::Unchecked;
        }
        break;
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
        switch (index.column())
        {
        case ModColumn:
            break;
        case NameColumn:
            return mod->modInfo().name();
//        case PatchColumn:
//            return mod->modInfo().patch();
//        case TypeColumn:
//            return mod->modInfo().type();
        case GameVersionColumn:
            return mod->modInfo().gameVersion().toString();
        case FileNameColumn:
            return mod->modInfo().fileName();
        case PreviewColumn:
            break;
        case MirrorLinkColumn:
            return mod->modInfo().mirrorUrl();
        }
        break;
    case Qt::DecorationRole:
        switch (index.column())
        {
        case NameColumn:
            return QIcon(":/image/optifine.png");
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

void OptifineManagerModel::setItemHeight(int newItemHeight)
{
    itemHeight_ = newItemHeight;
}

OptifineManagerProxyModel::OptifineManagerProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{}

void OptifineManagerProxyModel::setGameVersion(const GameVersion &newGameVersion)
{
    gameVersion_ = newGameVersion;
}

bool OptifineManagerProxyModel::filterAcceptsRow(int source_row, const QModelIndex &) const
{
    auto mod = sourceModel()->index(source_row, OptifineManagerModel::ModColumn).data(Qt::UserRole + 1).value<OptifineMod *>();
    if(gameVersion_ != GameVersion::Any && gameVersion_ != mod->modInfo().gameVersion()) return false;
    if(!mod->modInfo().fileName().contains(text_)) return false;
    if(!showPreview_ && mod->modInfo().isPreview()) return false;
    return true;
}

void OptifineManagerProxyModel::setShowPreview(bool newShowPreview)
{
    showPreview_ = newShowPreview;
}

void OptifineManagerProxyModel::setText(const QString &newText)
{
    text_ = newText;
}
