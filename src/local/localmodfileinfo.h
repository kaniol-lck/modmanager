#ifndef LOCALMODFILEINFO_H
#define LOCALMODFILEINFO_H

#include <QString>
#include <QMap>
#include <QFileInfo>

#include "fabricmodinfo.h"
#include "modloadertype.h"

class LocalModFile;

class LocalModFileInfo
{
    friend class LocalModFile;
public:
    LocalModFileInfo() = default;

    bool operator==(const LocalModFileInfo &other) const;

    void addOld();
    void removeOld();

    //common info
    QString id() const;
    QString name() const;
    QString version() const;
    QString description() const;
    QByteArray iconBytes() const;
    QStringList authors() const;
    QUrl website() const;
    QUrl sources() const;
    QUrl issues() const;

    ModLoaderType::Type loaderType() const;

    FabricModInfo fabric() const;

    const QList<FabricModInfo> &fabricModInfoList() const;

private:
    ModLoaderType::Type loaderType_ = ModLoaderType::Any;

    QList<FabricModInfo> fabricModInfoList_;

};

#endif // LOCALMODFILEINFO_H
