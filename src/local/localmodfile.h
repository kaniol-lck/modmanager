#ifndef LOCALMODFILE_H
#define LOCALMODFILE_H

#include <QObject>
#include <QFileInfo>

#include "tag/tagable.h"
#include "updater.hpp"

#include "fabricmodinfo.h"
#include "forgemodinfo.h"
#include "modloadertype.h"

class LocalModPath;
class LocalMod;
class LocalFileLinker;
class LocalModFile : public QObject, public Tagable
{
    Q_OBJECT
public:
    //TODO: subdir
    explicit LocalModFile(LocalModPath *parent, const QString &path, const QStringList &subDirs = QStringList());
    ~LocalModFile();

    ModLoaderType::Type loadInfo();

    bool moveTo(LocalModPath *path);

    bool remove();
    bool rename(const QString newBaseName);

    bool addOld();
    bool removeOld();

    bool setEnabled(bool enabled);

    const QString &sha1() const;
    const QString &murmurhash() const;
    std::tuple<QString, QString> baseNameFullSuffix() const;

    const static QStringList availableSuffix;

    enum FileType {
        Normal,
        Old,
        Disabled,
        Downloading,
        NotMod
    };

    FileType type() const;

    const QString &path() const;
    const QFileInfo &fileInfo() const;

    const CommonModInfo *commonInfo() const;

    FabricModInfo fabric() const;
    ForgeModInfo forge() const;

    ModLoaderType::Type loaderType() const;
    QList<ModLoaderType::Type> loaderTypes() const;
    void setLoaderType(ModLoaderType::Type newLoaderType);
    const QList<FabricModInfo> &fabricModInfoList() const;
    const QList<ForgeModInfo> &forgeModInfoList() const;

    const QStringList &subDirs() const;

    LocalFileLinker *linker() const;

    LocalMod *mod() const;
    void setMod(LocalMod *newMod);

    LocalModPath *modPath() const;
    void setModPath(LocalModPath *newModPath);

signals:
    void fileChanged();

private slots:
    void updateFileNameTags();

private:
    // 路径信息的唯一入口：改名 / 移动 / 启用停用 / 转 old 都必须走这里，
    // 否则 path_ 与 fileInfo_ 会与实际文件位置脱节（列表显示不存在的旧文件、
    // "Show in Folder" 打开旧目录、下次更新又写回旧目录）。
    void setPath(const QString &newPath);

    LocalFileLinker *linker_;
    LocalModPath *modPath_ = nullptr;
    LocalMod *mod_ = nullptr;

    //file
    QString path_;
    QStringList subDirs_;
    QFileInfo fileInfo_;
    QString sha1_;
    QString murmurhash_;

    //mod
    ModLoaderType::Type loaderType_ = ModLoaderType::Any;
    QList<FabricModInfo> fabricModInfoList_;
    QList<ForgeModInfo> forgeModInfoList_;
};

#endif // LOCALMODFILE_H
