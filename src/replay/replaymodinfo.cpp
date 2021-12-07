#include "replaymodinfo.h"

#include <QDebug>

#include "util/funcutil.h"

ReplayModInfo ReplayModInfo::fromHtml(const QString &html, const QString &gameVersionString)
{
    ReplayModInfo info;
    info.name_ = capture(html, "<th>(Replay Mod .*?) ");
    info.fileName_ = "replaymod-" + capture(info.name_, "Replay Mod v(.*?)$") + ".jar";
    info.urlPath_ = capture(html, R"_(<td><a href="([^"]*?)".*?>(<.*?>)*?Download.*?</a></td>)_");
    info.gameVersionString_ = gameVersionString;
    info.gameVersion_ = GameVersion::deduceFromString(gameVersionString);
    info.loaderType_ = ModLoaderType::fromString(capture(html, R"_(\(requires <a href=".*?">(.*?)</a>\))_"));
    info.downloadCount_ = capture(html, R"(<td>(\d+?) Downloads</td>)").toInt();
    return info;
}

const QString &ReplayModInfo::name() const
{
    return name_;
}

const QString &ReplayModInfo::urlPath() const
{
    return urlPath_;
}

const QString &ReplayModInfo::fileName() const
{
    return fileName_;
}

ModLoaderType::Type ReplayModInfo::loaderType() const
{
    return loaderType_;
}

int ReplayModInfo::downloadCount() const
{
    return downloadCount_;
}

const QString &ReplayModInfo::gameVersionString() const
{
    return gameVersionString_;
}

const GameVersion &ReplayModInfo::gameVersion() const
{
    return gameVersion_;
}
