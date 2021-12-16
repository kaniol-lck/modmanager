#ifndef DOWNLOADFILEINFO_H
#define DOWNLOADFILEINFO_H

#include <QString>
#include <QUrl>

#include "curseforge/curseforgefileinfo.h"
#include "modrinth/modrinthfileinfo.h"
#include "optifine/optifinemodinfo.h"
#include "replay/replaymodinfo.h"
#include "github/githubfileinfo.h"

class DownloadFileInfo
{
public:
    enum SourceType{ Curseforge, Modrinth, Custom };

    DownloadFileInfo() = default;
    explicit DownloadFileInfo(const QUrl &url);

    //implicit convert
    DownloadFileInfo(const CurseforgeFileInfo &info);
    DownloadFileInfo(const ModrinthFileInfo &info);
    DownloadFileInfo(const OptifineModInfo &info);
    DownloadFileInfo(const ReplayModInfo &info);
    DownloadFileInfo(const GitHubFileInfo &info);

    const QString &fileName() const;
    const QUrl &url() const;
    qint64 size() const;

    const QString &path() const;
    void setPath(const QString &newPath);

    const QPixmap &icon() const;
    void setIcon(const QPixmap &newIcon);

    void setTitle(const QString &newTitle);
    void setIconBytes(const QByteArray &newIconBytes);

    const QString &title() const;
    const QString &displayName() const;

protected:
    QString title_;
    QString displayName_;
    QString fileName_;
    QUrl url_;
    qint64 size_ = 0;
    QString path_;
    QPixmap icon_;
};

#endif // DOWNLOADFILEINFO_H
