#ifndef MODDIRINFO_H
#define MODDIRINFO_H

#include <QDir>

class ModDirInfo
{
public:
    ModDirInfo(QDir dir, QString version);

    bool exists() const;

    const QDir &getModDir() const;
    const QString &getGameVersion() const;

private:
    QDir modDir;
    QString gameVersion;
};

#endif // MODDIRINFO_H
