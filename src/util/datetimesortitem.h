#ifndef CURSEFORGEFILELISTWIDGETITEM_H
#define CURSEFORGEFILELISTWIDGETITEM_H

#include <QListWidgetItem>

class DateTimeSortItem : public QListWidgetItem
{
public:
    DateTimeSortItem();

    static constexpr int Role = 777;

    virtual bool operator<(const QListWidgetItem &other) const;
};

#endif // CURSEFORGEFILELISTWIDGETITEM_H
