#ifndef MODFILERENAMER_H
#define MODFILERENAMER_H

#include <QString>

class LocalMod;
class ModFileRenamer
{
public:
    explicit ModFileRenamer(const QString &renamePattern = QString());

    const QString &renamePattern() const;
    void setRenamePattern(const QString &renamePattern);

    QString newBaseFileName(LocalMod *mod);
    void renameMod(LocalMod *mod);
    void renameMods(QList<LocalMod *> mods);
private:
    QString renamePattern_;
};

#endif // MODFILERENAMER_H
