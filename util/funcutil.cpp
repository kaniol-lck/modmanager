#include "funcutil.h"

QString numberConvert(int size, const QString &suffix, int prec, int limit){
    if(size < limit)
        return QString::number(size, 'g', prec) + suffix;
    else if(size < limit * 1000)
        return QString::number(size / 1000., 'g', prec) + "K" + suffix;
    else if(size < limit * 1000000)
        return QString::number(size / 1000000., 'g', prec) + "M" + suffix;
    else
        return QString::number(size / 1000000., 'g', prec) + "G" + suffix;
}
