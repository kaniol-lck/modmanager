#ifndef LOCALMODFILE_H
#define LOCALMODFILE_H

#include <QObject>

#include "localmodfileinfo.h"

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
    const LocalModFileInfo &modInfo() const;

signals:
    void fileChanged();

private:
    QString path_;
    QFileInfo fileInfo_;
    LocalModFileInfo modInfo_;

    QString sha1_;
    QString murmurhash_;

};

#endif // LOCALMODFILE_H
