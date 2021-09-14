#ifndef LOCALMODINFO_H
#define LOCALMODINFO_H

#include <QString>
#include <QMap>
#include <QFileInfo>

#include "fabricmodinfo.h"
#include "modloadertype.h"

class LocalModInfo
{
public:
    LocalModInfo() = default;
    explicit LocalModInfo(QString path);

    bool operator==(const LocalModInfo &other) const;

    bool rename(const QString &newBaseName);

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

    //file related
    const QString &path() const;
    const QFileInfo &fileInfo() const;
    const QString &sha1() const;
    const QString &murmurhash() const;

    ModLoaderType::Type loaderType() const;

    FabricModInfo fabric() const;

    const QList<FabricModInfo> &fabricModInfoList() const;

private:
    QString path_;
    QFileInfo fileInfo_;
    ModLoaderType::Type loaderType_ = ModLoaderType::Any;

    QList<FabricModInfo> fabricModInfoList_;

    QString sha1_;
    QString murmurhash_;

};

#endif // LOCALMODINFO_H
