#ifndef LOCALMODINFO_H
#define LOCALMODINFO_H

#include <QString>
#include <QFileInfo>

class LocalModInfo
{
public:
    explicit LocalModInfo(QString path);

    bool acquireInfo(QString &path);

    bool isFabricMod() const;

    const QString &id() const;
    const QString &path() const;
    const QString &name() const;
    const QString &version() const;
    const QString &description() const;
    const QString &sha1() const;
    const QString &murmurhash() const;
    const QByteArray &iconBytes() const;

    const QStringList &authors() const;

    const QFileInfo &fileInfo() const;

private:
    QString path_;
    QFileInfo fileInfo_;
    QString id_;
    QString name_;
    QString version_;
    QStringList authors_;
    QString description_;
    QString sha1_;
    QString murmurhash_;
    QByteArray iconBytes_;

    bool hasFabricManifest_ = false;


};

#endif // LOCALMODINFO_H
