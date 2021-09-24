#ifndef REPLAYMODINFO_H
#define REPLAYMODINFO_H

#include <QUrl>

#include "gameversion.h"
#include "modloadertype.h"

class ReplayMod;

class ReplayModInfo
{
    friend class ReplayMod;
public:
    ReplayModInfo() = default;
    static ReplayModInfo fromHtml(const QString &html, const QString &gameVersionString = GameVersion::Any);

    const QString &name() const;
    const QString &urlPath() const;
    const QString &fileName() const;
    const QString &gameVersionString() const;
    const GameVersion &gameVersion() const;
    ModLoaderType::Type loaderType() const;
    int downloadCount() const;

private:
    QString name_;
    QString urlPath_;
    QString fileName_;
    QString gameVersionString_;
    GameVersion gameVersion_;
    ModLoaderType::Type loaderType_;

    int downloadCount_;
};

#endif // REPLAYMODINFO_H
