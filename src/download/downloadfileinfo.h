#ifndef DOWNLOADFILEINFO_H
#define DOWNLOADFILEINFO_H

#include <QString>
#include <QUrl>

class DownloadFileInfo
{
public:
    enum SourceType{ Curseforge, Modrinth, Custom };

    DownloadFileInfo();
    virtual ~DownloadFileInfo() = default;

    virtual SourceType source() const = 0;

    const QString &displayName() const;
    const QString &fileName() const;
    const QUrl &url() const;
    qint64 size() const;

protected:
    QString displayName_;
    QString fileName_;
    QUrl url_;
    qint64 size_;
};

#endif // DOWNLOADFILEINFO_H
