#include "optifinemodinfo.h"

#include <QDebug>

#include "util/funcutil.h"

OptifineModInfo OptifineModInfo::fromHtml(const QString &html)
{
    OptifineModInfo info;
    info.name_ = capture(html, R"(<td class='colFile'>(.*)</td>)");
    info.mirrorUrl_ = capture(html, R"_(<td class='colMirror'><a href="(.*)">.*</a></td>)_");
    info.fileName_ = capture(info.mirrorUrl_.toString(), R"(=(.*\.jar))");
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

const QUrl &OptifineModInfo::mirrorUrl() const
{
    return mirrorUrl_;
}

const QUrl &OptifineModInfo::downloadUrl() const
{
    return downloadUrl_;
}
