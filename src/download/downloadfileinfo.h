#ifndef DOWNLOADFILEINFO_H
#define DOWNLOADFILEINFO_H

#include <QString>
#include <QUrl>

#include "curseforge/curseforgefileinfo.h"
#include "modrinth/modrinthfileinfo.h"
#include "optifine/optifinemodinfo.h"
#include "replay/replaymodinfo.h"

class DownloadFileInfo
{
    friend class ModDownloader;
public:
    enum SourceType{ Curseforge, Modrinth, Custom };

    DownloadFileInfo() = default;
    explicit DownloadFileInfo(const QUrl &url);

    //implicit convert
    DownloadFileInfo(const CurseforgeFileInfo &info);
    DownloadFileInfo(const ModrinthFileInfo &info);
    DownloadFileInfo(const OptifineModInfo &info);
    DownloadFileInfo(const ReplayModInfo &info);

    static DownloadFileInfo fromCurseforge(const CurseforgeFileInfo &info);
    static DownloadFileInfo fromModrinth(const ModrinthFileInfo &info);

    const QString &displayName() const;
    const QString &fileName() const;
    const QUrl &url() const;
    qint64 size() const;

    const QString &path() const;
    void setPath(const QString &newPath);

    const QByteArray &iconBytes() const;
    void setIconBytes(const QByteArray &newIconBytes);


protected:
    QString displayName_;
    QString fileName_;
    QUrl url_;
    qint64 size_ = 0;
    QString path_;
    QByteArray iconBytes_;
};

#endif // DOWNLOADFILEINFO_H
