#include "optifinemodinfo.h"

#include <QDebug>

#include "util/funcutil.h"

OptifineModInfo OptifineModInfo::fromHtml(const QString &html, const GameVersion &gameVersion)
{
    OptifineModInfo info;
    info.name_ = capture(html, R"(<td class='colFile'>(.*)</td>)");
    info.mirrorUrl_ = capture(html, R"_(<td class='colMirror'><a href="(.*)">.*</a></td>)_");
    info.fileName_ = capture(info.mirrorUrl_.toString(), R"(=(.*\.jar))");
    info.gameVersion_ = gameVersion;
    info.isPreview_ = info.fileName_.contains("preview");
    return info;
}

const QString &OptifineModInfo::name() const
{
    return name_;
}

const QString &OptifineModInfo::fileName() const
{
    return fileName_;
}

const GameVersion &OptifineModInfo::gameVersion() const
{
    return gameVersion_;
}

const QUrl &OptifineModInfo::mirrorUrl() const
{
    return mirrorUrl_;
}

const QUrl &OptifineModInfo::downloadUrl() const
{
    return downloadUrl_;
}

bool OptifineModInfo::isPreview() const
{
    return isPreview_;
}

void OptifineModInfo::setGameVersion(const GameVersion &newGameVersion)
{
    gameVersion_ = newGameVersion;
}
