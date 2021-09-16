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
};

#endif // FORGEMODINFO_H
