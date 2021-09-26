#ifndef MODDOWNLOADER_H
#define MODDOWNLOADER_H

#include "downloader.h"
#include "downloadfileinfo.h"

class ModDownloader : public Downloader
{
    Q_OBJECT
public:
    explicit ModDownloader(QObject *parent = nullptr);

    void downloadMod(const DownloadFileInfo &info);
    void updateMod(const DownloadFileInfo &info);

    const DownloadFileInfo &fileInfo() const;

private:
    DownloadFileInfo fileInfo_;
};

#endif // MODDOWNLOADER_H
