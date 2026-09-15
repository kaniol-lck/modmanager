#include "optifinemodinfo.h"

#include <QDebug>

#include "util/funcutil.h"
#include "util/tutil.hpp"

OptifineModInfo OptifineModInfo::fromHtml(const QString &html, const GameVersion &gameVersion)
{
    OptifineModInfo info;
    // 属性引号一律用 ["'] 兼容：原来把单/双引号写死，站点一改引号风格整列都会解析不出来
    info.name_ = capture(html, R"(<td class=["']colFile["']>(.*?)</td>)");
    info.mirrorUrl_ = capture(html, R"_(<td class=["']colMirror["']><a href=["'](.*?)["']>.*?</a></td>)_");
    info.fileName_ = capture(info.mirrorUrl_.toString(), R"(=(.*?\.jar))");
    info.gameVersion_ = gameVersion;
    info.isPreview_ = info.fileName_.contains("preview");
    return info;
}

OptifineModInfo OptifineModInfo::fromVariant(const QVariant &variant)
{
    OptifineModInfo info;
    info.type_ = value(variant, "type").toString();
    info.patch_ = value(variant, "patch").toString();
    QStringList list;
    list << "Optifine"
         << QString(info.type_).replace("_", " ")
         << info.patch_;
    info.name_ = list.join(" ");
    // BMCLAPI 的 versionList 里本来就带 filename（如 OptiFine_1.13.2_HD_U_E7.jar），
    // 原实现无视它、拿显示名拼了一个 "Optifine_HD_U_E7.jar"：既丢了游戏版本、
    // 大小写也不对。后果是存盘文件名是错的，hasFile() 的"已下载"判断永远不成立
    // （按钮点了还在），本地 mod 也永远匹配不上这个文件。
    info.fileName_ = value(variant, "filename").toString();
    if(info.fileName_.isEmpty())
        info.fileName_ = QString(info.name_).replace(" ", "_") + ".jar";
    info.gameVersion_ = value(variant, "mcversion").toString();
    // 预览版判据同样优先用 filename（BMCLAPI 用 "preview_" 前缀），拿不到再退回 patch 里的 "pre"
    info.isPreview_ = info.fileName_.contains("preview") || info.patch_.contains("pre");
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

void OptifineModInfo::setMirrorUrl(const QUrl &newMirrorUrl)
{
    mirrorUrl_ = newMirrorUrl;
}

void OptifineModInfo::setDownloadUrl(const QUrl &newDownloadUrl)
{
    downloadUrl_ = newDownloadUrl;
}

const QString &OptifineModInfo::type() const
{
    return type_;
}

const QString &OptifineModInfo::patch() const
{
    return patch_;
}
