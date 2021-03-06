#ifndef LOCALMODPATHMODEL_H
#define LOCALMODPATHMODEL_H

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QElapsedTimer>

class LocalMod;
class LocalModFilter;
class LocalModPath;
class LocalModPathModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum c { ModColumn, NameColumn, IdColumn, VersionColumn, EnableColumn, StarColumn, TagsColumn, FileDateColumn, FileSizeColumn, FileNameColumn, CurseforgeIdColumn, CurseforgeFileIdColumn, ModrinthIdColumn, ModrinthFileIdColumn, DescriptionColumn };
    explicit LocalModPathModel(LocalModPath *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

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
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    void setItemHeight(int newItemHeight);

private:
    QElapsedTimer timer_;
    LocalModPath *path_;
    int itemHeight_ = 100;
};

class LocalModPathFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    LocalModPathFilterProxyModel(QObject *parent = nullptr);

    void setFilter(LocalModFilter *filter);
    void setText(const QString &newText);

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &) const override;
    virtual bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private:
    LocalModFilter *filter_ = nullptr;
    QString text_;
};

#endif // LOCALMODPATHMODEL_H
