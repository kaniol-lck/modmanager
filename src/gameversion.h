#ifndef GAMEVERSION_H
#define GAMEVERSION_H

#include <QString>
#include <optional>

class GameVersion
{
public:
    GameVersion() = default;
    GameVersion(const QString &string);

    GameVersion mainVersion() const;

    const QString &getVersionString() const;

    operator QString() const;
    bool operator==(const GameVersion &other) const;
    bool operator!=(const GameVersion &other) const;

    static std::optional<GameVersion> deduceFromString(const QString &string);

    static void initVersionList();

    static QStringList cachedVersionList;
    static QStringList curseforgeVersionList;

    static GameVersion Any;

private:
    QString versionString_;
};

#endif // GAMEVERSION_H
