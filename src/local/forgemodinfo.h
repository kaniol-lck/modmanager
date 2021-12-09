#ifndef FORGEMODINFO_H
#define FORGEMODINFO_H

#include "commonmodinfo.h"

class QuaZip;

class ForgeModInfo : public CommonModInfo
{
public:
    ForgeModInfo() = default;
    static QList<ForgeModInfo> fromZip(const QString &path);
    static QList<ForgeModInfo> fromZip(QuaZip *zip);

private:
    static QString optiFineVersion(QuaZip *zip);

    QString credits_;
    QUrl updateUrl_;
};

#endif // FORGEMODINFO_H
