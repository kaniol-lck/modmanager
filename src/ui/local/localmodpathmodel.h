#ifndef LOCALMODPATHMODEL_H
#define LOCALMODPATHMODEL_H

#include <QAbstractItemModel>

class LocalModPath;
class LocalModPathModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum c { ModColumn, NameColumn, IdColumn, VersionColumn, EnableColumn, StarColumn, TagsColumn, FileDateColumn, FileSizeColumn, FileNameColumn, CurseforgeIdColumn, CurseforgeFileIdColumn, ModrinthIdColumn, ModrinthFileIdColumn, DescriptionColumn };
    explicit LocalModPathModel(LocalModPath *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // Fetch data dynamically:
//    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

//    bool canFetchMore(const QModelIndex &parent) const override;
//    void fetchMore(const QModelIndex &parent) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    LocalModPath *path_;
};

#endif // LOCALMODPATHMODEL_H
