#ifndef LOCALMODITEM_H
#define LOCALMODITEM_H

#include <QStandardItem>

class LocalMod;
class LocalModItem : public QStandardItem
{
    //custom sorters
    static constexpr int FileDateRole = Qt::UserRole + 147;
    static constexpr int FileSizeRole = Qt::UserRole + 148;
public:
    LocalModItem(LocalMod *mod = nullptr);
    bool operator<(const QStandardItem &other) const override;

    static QList<QStandardItem *> itemsFromMod(LocalMod *mod);
private:
    LocalMod *mod_;
};

#endif // LOCALMODITEM_H
