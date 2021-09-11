#ifndef LOCALMODINFO_H
#define LOCALMODINFO_H

#include <QString>
#include <QMap>
#include <QFileInfo>

class LocalModInfo
{
public:
    explicit LocalModInfo(QString path);

    bool operator==(const LocalModInfo &other) const;

    bool isFabricMod() const;

    void addOld();
    void removeOld();

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

    const QMap<QString, QString> &depends() const;
    const QMap<QString, QString> &conflicts() const;
    const QMap<QString, QString> &breaks() const;

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
    QMap<QString, QString> depends_;
    QMap<QString, QString> conflicts_;
    QMap<QString, QString> breaks_;

    bool isMod_ = false;
    bool hasFabricManifest_ = false;


};

#endif // LOCALMODINFO_H
