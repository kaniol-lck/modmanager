#ifndef GAMEVERSION_H
#define GAMEVERSION_H

#include <QString>
#include <optional>

class GameVersion
{
public:
    GameVersion(const QString string);

    const QString &getVersionString() const;

    operator QString() const;
    bool operator==(GameVersion& another) const;
    bool operator!=(GameVersion& another) const;

    static std::optional<GameVersion> deduceFromString(const QString &string);

    static void initVersionList();

    static QStringList versionList;

    static GameVersion ANY;

private:
    QString versionString;
};

#endif // GAMEVERSION_H
