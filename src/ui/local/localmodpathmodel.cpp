#include "localmodfilter.h"
#include "localmodpathmodel.h"

#include "local/localmodpath.h"
#include "curseforge/curseforgemod.h"
#include "modrinth/modrinthmod.h"
#include "util/funcutil.h"

#include <QBrush>

LocalModPathModel::LocalModPathModel(LocalModPath *parent) :
    QAbstractItemModel(parent),
    path_(parent)
{
    connect(path_, &LocalModPath::modListUpdated, this, [=]{
        beginResetModel();
        endResetModel();
    });
}

QVariant LocalModPathModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation != Qt::Horizontal) return QVariant();
    switch (role) {
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
        switch (section)
        {
        case LocalModPathModel::ModColumn:
            return QVariant();
        case LocalModPathModel::NameColumn:
            return tr("Mod Name");
        case LocalModPathModel::IdColumn:
            return tr("ID");
        case LocalModPathModel::VersionColumn:
            return tr("Version");
        case LocalModPathModel::EnableColumn:
            return tr("Enable");
        case LocalModPathModel::StarColumn:
            return tr("Star");
        case LocalModPathModel::TagsColumn:
            return tr("Tags");
        case LocalModPathModel::FileDateColumn:
            return tr("Last Modified");
        case LocalModPathModel::FileSizeColumn:
            return tr("File Size");
        case LocalModPathModel::FileNameColumn:
            return tr("File Name");
        case LocalModPathModel::CurseforgeIdColumn:
            return tr("Curseforge ID");
        case LocalModPathModel::CurseforgeFileIdColumn:
            return tr("Curseforge File ID");
        case LocalModPathModel::ModrinthIdColumn:
            return tr("Modrinth ID");
        case LocalModPathModel::ModrinthFileIdColumn:
            return tr("Modrinth File ID");
        case LocalModPathModel::DescriptionColumn:
            return tr("Description");
        }
        break;
    case Qt::DecorationRole:
        switch (section)
        {
        case LocalModPathModel::StarColumn:
            return QIcon::fromTheme("starred-symbolic");
        case LocalModPathModel::TagsColumn:
            return QIcon::fromTheme("tag");
        case LocalModPathModel::CurseforgeIdColumn:
        case LocalModPathModel::CurseforgeFileIdColumn:
            return QIcon(":/image/curseforge.svg");
        case LocalModPathModel::ModrinthIdColumn:
        case LocalModPathModel::ModrinthFileIdColumn:
            return QIcon(":/image/modrinth.svg");
        }
        break;
    }
    return QVariant();
}

Qt::ItemFlags LocalModPathModel::flags(const QModelIndex &index) const
{
    switch (index.column()) {
    case EnableColumn:
    case StarColumn:
        return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
    default:
        return QAbstractItemModel::flags(index);
    }
}

QModelIndex LocalModPathModel::index(int row, int column, const QModelIndex &parent) const
{
    if(parent.isValid())
        return QModelIndex();
    return createIndex(row, column);
}

QModelIndex LocalModPathModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int LocalModPathModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return path_->modMap().count();
}

int LocalModPathModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return DescriptionColumn + 1;
}

//bool LocalModPathModel::hasChildren(const QModelIndex &parent) const
//{
//    // FIXME: Implement me!
//    return {};
//}

//bool LocalModPathModel::canFetchMore(const QModelIndex &parent) const
//{
//    // FIXME: Implement me!
//    return false;
//}

//void LocalModPathModel::fetchMore(const QModelIndex &parent)
//{
//    // FIXME: Implement me!
//}

QVariant LocalModPathModel::data(const QModelIndex &index, int role) const
{
//    qDebug() << index << role;
    if (!index.isValid() || index.row() >= rowCount())
        return QVariant();
    auto mod = *(path_->modMap().begin() + index.row());

    switch (role) {
    case Qt::CheckStateRole:
        switch (index.column()) {
        case EnableColumn:
            return mod->isEnabled()? Qt::Checked : Qt::Unchecked;
        case StarColumn:
            return mod->isFeatured()? Qt::Checked : Qt::Unchecked;
        }
        break;
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
        switch (index.column())
        {
        case LocalModPathModel::ModColumn:
            return "";
        case LocalModPathModel::NameColumn:
            return clearFormat(mod->displayName());
        case LocalModPathModel::IdColumn:
            return mod->commonInfo()->id();
        case LocalModPathModel::VersionColumn:
            return mod->commonInfo()->version();
        case LocalModPathModel::EnableColumn:
            return {};//mod->isEnabled();
        case LocalModPathModel::StarColumn:
            return {};//mod->isFeatured();
        case LocalModPathModel::TagsColumn:
            return "";
        case LocalModPathModel::FileDateColumn:
            return mod->modFile()->fileInfo().lastModified().toString();
        case LocalModPathModel::FileSizeColumn:
            return sizeConvert(mod->modFile()->fileInfo().size());
        case LocalModPathModel::FileNameColumn:
            return mod->modFile()->fileInfo().fileName();
        case LocalModPathModel::CurseforgeIdColumn:
            return mod->curseforgeMod()? mod->curseforgeMod()->modInfo().id() : QVariant();
        case LocalModPathModel::CurseforgeFileIdColumn:
            return mod->modFile()->linker()->curseforgeFileInfo()? mod->modFile()->linker()->curseforgeFileInfo()->id() : QVariant();
        case LocalModPathModel::ModrinthIdColumn:
            return mod->modrinthMod()? mod->modrinthMod()->modInfo().id() : QVariant();
        case LocalModPathModel::ModrinthFileIdColumn:
            return mod->modFile()->linker()->modrinthFileInfo()? mod->modFile()->linker()->modrinthFileInfo()->id() : QVariant();
        case LocalModPathModel::DescriptionColumn:
            return mod->commonInfo()->description();
        }
        break;
    case Qt::DecorationRole:
        switch (index.column())
        {
        case NameColumn:
            return QIcon(mod->icon().scaled(96, 96, Qt::KeepAspectRatio));
        }
        break;
    case Qt::ForegroundRole:{
        QBrush brush;
        brush.setColor(mod->isDisabled()? Qt::gray : Qt::black);
        return brush;
        break;
    }
    case Qt::TextAlignmentRole:
        switch (index.column())
        {
        case NameColumn:
            return QVariant();
        default:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        }
        break;
    case Qt::SizeHintRole:
        if(index.column() == ModColumn)
            return QSize(0, itemHeight_);
        break;
    case Qt::UserRole + 1:
        switch (index.column())
        {
        case ModColumn:
            return QVariant::fromValue(mod);
        case FileDateColumn:
            return mod->modFile()->fileInfo().lastModified();
        case FileSizeColumn:
            return mod->modFile()->fileInfo().size();
        }
        break;
    }
    return QVariant();
}

bool LocalModPathModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= rowCount())
        return false;
    auto mod = *(path_->modMap().begin() + index.row());
    switch (role) {
    case Qt::CheckStateRole:
        switch (index.column()) {
        case EnableColumn:
            mod->setEnabled(value.toBool());
            return true;
        case StarColumn:
            mod->setFeatured(value.toBool());
            return true;
        }
    }
    return false;
}

void LocalModPathModel::setItemHeight(int newItemHeight)
{
    itemHeight_ = newItemHeight;
}

LocalModPathFilterProxyModel::LocalModPathFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{}

void LocalModPathFilterProxyModel::setFilter(LocalModFilter *filter)
{
    filter_ = filter;
}

bool LocalModPathFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &) const
{
    auto mod = sourceModel()->index(source_row, LocalModPathModel::ModColumn).data(Qt::UserRole + 1).value<LocalMod *>();
    return filter_->willShow(mod, text_);
}

void LocalModPathFilterProxyModel::setText(const QString &newText)
{
    text_ = newText;
}
