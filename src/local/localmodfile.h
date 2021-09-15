#ifndef LOCALMODFILE_H
#define LOCALMODFILE_H

#include <QObject>

#include "localmodinfo.h"

class LocalModFile : public QObject
{
    Q_OBJECT
public:
    explicit LocalModFile(QObject *parent, const QString &path);

    void loadInfo();

    bool rename(const QString newBaseName);

    bool addOld();
    bool removeOld();

    const QString &path() const;
    const QFileInfo &fileInfo() const;
    const QString &sha1() const;
    const QString &murmurhash() const;
    std::tuple<QString, QString> baseNameFullSuffix() const;

    const static QStringList availableSuffix;

signals:
    void fileChanged();

private:
    QString path_;
    QFileInfo fileInfo_;
    LocalModInfo modInfo_;

};

#endif // LOCALMODFILE_H
