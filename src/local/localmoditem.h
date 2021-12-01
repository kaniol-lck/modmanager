#ifndef LOCALMODITEM_H
#define LOCALMODITEM_H

#include <QStandardItem>

class LocalMod;
class LocalModItem : public QStandardItem
{
    //custom sorters
    static constexpr int FileDateRole = Qt::UserRole + 147;
    static constexpr int FileSizeRole = Qt::UserRole + 148;
    static constexpr int TagsRole = Qt::UserRole + 149;
public:
    enum { ModColumn, NameColumn, IdColumn, VersionColumn, EnableColumn, StarColumn, TagsColumn, FileDateColumn, FileSizeColumn, FileNameColumn, CurseforgeIdColumn, CurseforgeFileIdColumn, ModrinthIdColumn, ModrinthFileIdColumn, DescriptionColumn };
    LocalModItem(LocalMod *mod = nullptr);
    bool operator<(const QStandardItem &other) const override;

    static QList<QStandardItem *> itemsFromMod(LocalMod *mod);
    static QList<QPair<int, QStandardItem *> > headerItems();
private:
    LocalMod *mod_;
};

#endif // LOCALMODITEM_H
