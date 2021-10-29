#ifndef GAMEVERSION_H
#define GAMEVERSION_H

#include <QObject>
#include <QString>
#include <optional>

class GameVersion
{
    friend class VersionManager;
public:
    GameVersion() = default;
    GameVersion(const QString &string);
    GameVersion(int mainVersion, int majorVersion, int minorVersion = 0);

    GameVersion majorVersion() const;

    const QString &versionString() const;
    bool isDev() const;

    operator QString() const;
    QString toString() const;
    bool operator==(const GameVersion &other) const;
    bool operator!=(const GameVersion &other) const;

    static GameVersion deduceFromString(const QString &string);

    static GameVersion Any;

    static QList<GameVersion> mojangReleaseVersionList();
    static QList<GameVersion> curseforgeVersionList();
    static QList<GameVersion> modrinthVersionList();

private:
    QString versionString_;

    int mainVersionNumber_;
    int majorVersionNumber_;
    int minorVersionNumber_;

    static QList<GameVersion> cachedVersionList_;
    static QList<GameVersion> mojangVersionList_;
    static QList<GameVersion> mojangReleaseVersionList_;
    static QList<GameVersion> curseforgeVersionList_;
};

class VersionManager : public QObject
{
    Q_OBJECT
public:
    VersionManager() = default;
    ~VersionManager() = default;
    static VersionManager *manager();

    static void initVersionLists();

signals:
    void mojangVersionListUpdated();
    void modrinthVersionListUpdated();
    void curseforgeVersionListUpdated();

private:
    void initMojangVersionList();
    void initCurseforgeVersionList();
};
#endif // GAMEVERSION_H
