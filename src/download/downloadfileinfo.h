#ifndef DOWNLOADFILEINFO_H
#define DOWNLOADFILEINFO_H

#include <QString>
#include <QUrl>

#include "curseforge/curseforgefileinfo.h"
#include "modrinth/modrinthmodinfo.h"

class DownloadFileInfo
{
public:
    enum SourceType{ Curseforge, Modrinth, Custom };

    DownloadFileInfo() = default;
    explicit DownloadFileInfo(const QUrl &url);

    //implicit convert
    DownloadFileInfo(const CurseforgeFileInfo &info);
    DownloadFileInfo(const ModrinthFileInfo &info);

    static DownloadFileInfo fromCurseforge(const CurseforgeFileInfo &info);
    static DownloadFileInfo fromModrinth(const ModrinthFileInfo &info);

    const QString &displayName() const;
    const QString &fileName() const;
    const QUrl &url() const;
    qint64 size() const;

protected:
    QString displayName_;
    QString fileName_;
    QUrl url_;
    qint64 size_ = 0;
};

#endif // DOWNLOADFILEINFO_H
