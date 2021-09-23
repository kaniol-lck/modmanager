#ifndef OPTIFINEMODINFO_H
#define OPTIFINEMODINFO_H

#include <QUrl>

#include "gameversion.h"

class OptifineMod;

class OptifineModInfo
{
    friend class OptifineMod;
public:
    OptifineModInfo() = default;
    static OptifineModInfo fromHtml(const QString &html, const GameVersion &gameVersion = GameVersion::Any);

    const QString &name() const;
    const QString &fileName() const;
    const GameVersion &gameVersion() const;
    const QUrl &mirrorUrl() const;
    const QUrl &downloadUrl() const;
    bool isPreview() const;

    void setGameVersion(const GameVersion &newGameVersion);

private:
    QString name_;
    QString fileName_;
    GameVersion gameVersion_;
    QUrl mirrorUrl_;
    QUrl downloadUrl_;
    bool isPreview_;
};

#endif // OPTIFINEMODINFO_H
