#ifndef CURSEFORGEDEPENDENCYINFO_H
#define CURSEFORGEDEPENDENCYINFO_H

#include <QVariant>
#include <memory>

class CurseforgeFileInfo;
class CurseforgeDependencyInfo
{
public:
    enum Type { EmbeddedLibrary, Incompatible, OptionalDependency, RequiredDependency, Tool };
    CurseforgeDependencyInfo() = default;
    static CurseforgeDependencyInfo fromVariant(const QVariant &variant);

    int addonId() const;
    int fileId() const;
    int id() const;
    int type() const;

    QString typeString() const;

private:
    int addonId_ = 0;
    int fileId_ = 0;
    int id_ = 0;
    Type type_;
};

#endif // CURSEFORGEDEPENDENCYINFO_H
