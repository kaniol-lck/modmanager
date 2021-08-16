#include "funcutil.h"

QString filesize2String(int size, int prec){
    if(size < 850)
        return QString::number(size, 'g', prec) + " B";
    else if(size < 850000)
        return QString::number(size / 1000., 'g', prec) + " KiB";
    else if(size < 850000000)
        return QString::number(size / 1000000., 'g', prec) + " MiB";
    else
        return QString::number(size / 1000000., 'g', prec) + " GiB";
}
