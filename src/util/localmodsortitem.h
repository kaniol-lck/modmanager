#ifndef LOCALMODSORTITEM_H
#define LOCALMODSORTITEM_H

#include <QListWidgetItem>

class LocalMod;

class LocalModSortItem : public QListWidgetItem
{
public:
    LocalModSortItem(LocalMod *mod);

    enum SortRule { Name, Date, Size };

    virtual bool operator<(const QListWidgetItem &other) const;

    LocalMod *mod() const;

    void setSortRule(SortRule newSortRule);

private:
    LocalMod *mod_;
    SortRule sortRule_ = Name;
};

#endif // LOCALMODSORTITEM_H
