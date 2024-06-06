#ifndef CURSEFORGEMODINFO_H
#define CURSEFORGEMODINFO_H

#include <QVariant>
#include <QUrl>
#include <optional>
#include <modloadertype.h>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>

#include "tag/tagable.h"
#include "curseforgecategoryinfo.h"
#include "curseforgefileinfo.h"
#include "curseforgemodcacheinfo.h"
#include "modloadertype.h"

class CurseforgeMod;

class CurseforgeModInfo : public CurseforgeModCacheInfo, Tagable
{
    friend class CurseforgeMod;
public:
    struct Attachment{
        QString title;
        QString description;
        QUrl thumbnailUrl;
        QUrl url;
    };

    explicit CurseforgeModInfo(int addonId = 0);

    static CurseforgeModInfo fromVariant(const QVariant &variant);

    const QUrl &websiteUrl() const;
    const QStringList &authors() const;
    const QUrl &iconUrl() const;
    const QString &description() const;
    int downloadCount() const;
    const QList<CurseforgeFileInfo> &latestFiles() const;
    std::optional<CurseforgeFileInfo> latestFileInfo(const GameVersion &version, ModLoaderType::Type &loaderType) const;
    const QList<CurseforgeFileInfo> &allFileList() const;
    const QList<CurseforgeCategoryInfo> &categories() const;
    const QList<Attachment> &images() const;
    const QDateTime &dateModified() const;
    const QDateTime &dateCreated() const;
    const QDateTime &dateReleased() const;

    void setLatestFiles(const QList<CurseforgeFileInfo> &newLatestFiles);
    bool hasBasicInfo() const;

    bool fileCompleted() const;
    int totalFileCount() const;
    void setTotalFileCount(int newTotalFileCount);

    const QList<int> &latestFileIndexList() const;

    const QList<CurseforgeFileInfo> &latestIndexedFileList() const;

private:
    QUrl websiteUrl_;
    QList<Attachment> images_;
    QStringList authors_;
    QString description_;
    int downloadCount_;
    QList<CurseforgeFileInfo> latestFileList_;
    QList<CurseforgeFileInfo> latestIndexedFileList_;
    QList<int> latestFileIndexList_;
    QList<CurseforgeFileInfo> allFileList_;
    int totalFileCount_ = 0;
    QList<CurseforgeCategoryInfo> categories_;
    QDateTime dateModified_;
    QDateTime dateCreated_;
    QDateTime dateReleased_;

    bool basicInfo_ = false;
};

#endif // CURSEFORGEMODINFO_H
