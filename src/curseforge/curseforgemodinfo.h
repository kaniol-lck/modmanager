#ifndef CURSEFORGEMODINFO_H
#define CURSEFORGEMODINFO_H

#include <QVariant>
#include <QUrl>
#include <optional>
#include <modloadertype.h>

#include "curseforgefileinfo.h"
#include "modloadertype.h"

class CurseforgeMod;


class CurseforgeModInfo
{
    friend class CurseforgeMod;
public:
    CurseforgeModInfo() = default;
    explicit CurseforgeModInfo(int addonId);

    static CurseforgeModInfo fromVariant(const QVariant &variant);

    int id() const;
    const QString &name() const;
    const QString &summary() const;
    const QUrl &websiteUrl() const;
    const QStringList &authors() const;
    const QUrl &iconUrl() const;
    const QByteArray &iconBytes() const;
    const QString &description() const;
    int downloadCount() const;
    const QList<ModLoaderType::Type> &loaderTypes() const;
    bool isFabricMod() const;
    bool isForgeMod() const;
    bool isRiftMod() const;
    const QList<CurseforgeFileInfo> &latestFileList() const;
    std::optional<CurseforgeFileInfo> latestFileInfo(const GameVersion &version, ModLoaderType::Type &loaderType) const;
    const QList<CurseforgeFileInfo> &allFileList() const;
    const QList<int> &categories() const;

    void setLatestFiles(const QList<CurseforgeFileInfo> &newLatestFiles);
    bool hasBasicInfo() const;

private:
    int id_;
    QString name_;
    QString summary_;
    QUrl websiteUrl_;
    QStringList authors_;
    QUrl iconUrl_;
    QByteArray iconBytes_;
    QString description_;
    int downloadCount_;
    QList<ModLoaderType::Type> loaderTypes_;
    QList<CurseforgeFileInfo> latestFileList_;
    QList<CurseforgeFileInfo> allFileList_;
    QList<int> categories_;

    bool basicInfo_ = false;
};

#endif // CURSEFORGEMODINFO_H
