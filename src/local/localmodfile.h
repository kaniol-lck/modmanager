#ifndef LOCALMODFILE_H
#define LOCALMODFILE_H

#include <QObject>
#include <QFileInfo>

#include "tag/tagable.h"

#include "fabricmodinfo.h"
#include "forgemodinfo.h"
#include "modloadertype.h"

class LocalModFile : public QObject, public Tagable
{
    Q_OBJECT
public:
    //TODO: subdir
    explicit LocalModFile(QObject *parent, const QString &path, const QStringList &subDirs = QStringList());
    ~LocalModFile();

    ModLoaderType::Type loadInfo();

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
    QList<ModLoaderType::Type> loaderTypes() const;
    void setLoaderType(ModLoaderType::Type newLoaderType);
    const QList<FabricModInfo> &fabricModInfoList() const;
    const QList<ForgeModInfo> &forgeModInfoList() const;

    const QStringList &subDirs() const;

signals:
    void fileChanged();

private:
    //file
    QString path_;
    QStringList subDirs_;
    QFileInfo fileInfo_;
    QString sha1_;
    QString murmurhash_;

    //mod
    ModLoaderType::Type loaderType_ = ModLoaderType::Any;
    QList<FabricModInfo> fabricModInfoList_;
    QList<ForgeModInfo> forgeModInfoList_;
};

#endif // LOCALMODFILE_H
