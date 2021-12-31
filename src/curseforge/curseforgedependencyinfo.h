#ifndef CURSEFORGEDEPENDENCYINFO_H
#define CURSEFORGEDEPENDENCYINFO_H

#include <QVariant>
#include <memory>

class CurseforgeFileInfo;
class CurseforgeDependencyInfo
{
public:
    CurseforgeDependencyInfo() = default;
    static CurseforgeDependencyInfo fromVariant(const QVariant &variant);

    int addonId() const;
    int fileId() const;
    int id() const;
    int type() const;

private:
    int addonId_ = 0;
    int fileId_ = 0;
    int id_ = 0;
    int type_ = 0;
};

#endif // CURSEFORGEDEPENDENCYINFO_H
