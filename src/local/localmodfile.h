#ifndef LOCALMODFILE_H
#define LOCALMODFILE_H

#include <QObject>
#include <QFileInfo>

#include "fabricmodinfo.h"
#include "forgemodinfo.h"
#include "modloadertype.h"

class LocalModFile : public QObject
{
    Q_OBJECT
public:
    explicit LocalModFile(QObject *parent, const QString &path);

    bool loadInfo();

    bool remove();
    bool rename(const QString newBaseName);

    bool addOld();
    bool removeOld();

    bool setEnabled(bool enabled);

    const QString &sha1() const;
    const QString &murmurhash() const;
    std::tuple<QString, QString> baseNameFullSuffix() const;

    const static QStringList availableSuffix;

    enum FileType {
        Normal,
        Old,
        Disabled,
        Downloading,
        NotMod
    };

    FileType type() const;

    const QString &path() const;
    const QFileInfo &fileInfo() const;

    const CommonModInfo *commonInfo() const;

    FabricModInfo fabric() const;
    ForgeModInfo forge() const;

    ModLoaderType::Type loaderType() const;
    const QList<FabricModInfo> &fabricModInfoList() const;
    const QList<ForgeModInfo> &forgeModInfoList() const;

signals:
    void fileChanged();

private:
    //file
    QString path_;
    QFileInfo fileInfo_;
    QString sha1_;
    QString murmurhash_;

    //mod
    ModLoaderType::Type loaderType_ = ModLoaderType::Any;
    QList<FabricModInfo> fabricModInfoList_;
    QList<ForgeModInfo> forgeModInfoList_;
};

#endif // LOCALMODFILE_H
