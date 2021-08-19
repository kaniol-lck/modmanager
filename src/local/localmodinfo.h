#ifndef LOCALMODINFO_H
#define LOCALMODINFO_H

#include <QString>
#include <QFileInfo>

class LocalModInfo
{
public:
    LocalModInfo(QString path);

    bool isFabricMod();

    QDateTime getFileModificationTime() const;

    const QString &getId() const;
    const QString &getModPath() const;
    const QString &getName() const;
    const QString &getVersion() const;
    const QString &getDescription() const;
    const QString &getSha1() const;
    const QString &getMurmurhash() const;
    const QByteArray &getIconBytes() const;

private:
    QString modPath;
    QFileInfo modFileInfo;
    QString id;
    QString name;
    QString version;
    QString description;
    QString sha1;
    QString murmurhash;
    QByteArray iconBytes;

    bool hasFabricManifest = false;

    bool acquireInfo(QString &path);

};

#endif // LOCALMODINFO_H
