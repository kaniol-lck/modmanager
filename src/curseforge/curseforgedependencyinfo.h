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

    int modId() const;
    int type() const;

    QString typeString() const;

private:
    int modId_ = 0;
    Type type_;
};

#endif // CURSEFORGEDEPENDENCYINFO_H
