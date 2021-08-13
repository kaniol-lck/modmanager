#ifndef MODINFO_H
#define MODINFO_H

#include <QString>

class ModInfo
{
private:
    QString modPath;
    QString id;
    QString name;
    QString version;
    QString description;
    QString sha1;
    QString murmurhash;

    bool hasFabricManifest = false;

    bool acquireInfo(QString &path);

public:
    ModInfo(QString path);

    bool isFabricMod();

    const QString &getId() const;
    const QString &getModPath() const;
    const QString &getName() const;
    const QString &getVersion() const;
    const QString &getDescription() const;
    const QString &getSha1() const;
    const QString &getMurmurhash() const;
};

#endif // MODINFO_H
