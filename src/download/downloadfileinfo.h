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

    const QString &getDisplayName() const;
    const QString &getFileName() const;
    const QUrl &getUrl() const;
    qint64 getSize() const;

protected:
    QString displayName;
    QString fileName;
    QUrl url;
    qint64 size;
};

#endif // DOWNLOADFILEINFO_H
