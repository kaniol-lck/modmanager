#include "datetimesortitem.h"

#include <QDateTime>

DateTimeSortItem::DateTimeSortItem()
{

}

bool DateTimeSortItem::operator<(const QListWidgetItem &other) const
{
    return data(Role).toDateTime() < other.data(Role).toDateTime();
}
