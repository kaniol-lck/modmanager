#include "gameversion.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegExp>
#include <QJsonDocument>
#include <QDebug>

#include "curseforge/curseforgeapi.h"
#include "util/tutil.hpp"
#include "util/funcutil.h"
#include "config.hpp"

GameVersion GameVersion::Any = GameVersion("");

QList<GameVersion> GameVersion::mojangVersionList_;
QList<GameVersion> GameVersion::mojangReleaseVersionList_;
QList<GameVersion> GameVersion::curseforgeVersionList_;

QList<GameVersion> GameVersion::cachedVersionList_{
    GameVersion(1, 17, 1),
    GameVersion(1, 17),
    GameVersion(1, 16, 5),
    GameVersion(1, 16, 4),
    GameVersion(1, 16, 3),
    GameVersion(1, 16, 2),
    GameVersion(1, 16, 1),
    GameVersion(1, 16),
    GameVersion(1, 15, 2),
    GameVersion(1, 15, 1),
    GameVersion(1, 15),
    GameVersion(1, 14, 4),
    GameVersion(1, 14, 3),
    GameVersion(1, 14, 2),
    GameVersion(1, 14, 1),
    GameVersion(1, 14),
    GameVersion(1, 13, 2),
    GameVersion(1, 13, 1),
    GameVersion(1, 13),
    GameVersion(1, 12, 2),
    GameVersion(1, 12, 1),
    GameVersion(1, 12),
    GameVersion(1, 11, 2),
    GameVersion(1, 11, 1),
    GameVersion(1, 11),
    GameVersion(1, 10, 2),
    GameVersion(1, 10, 1),
    GameVersion(1, 10),
    GameVersion(1, 9, 4),
    GameVersion(1, 9, 3),
    GameVersion(1, 9, 2),
    GameVersion(1, 9, 1),
    GameVersion(1, 9),
    GameVersion(1, 8, 9),
    GameVersion(1, 8, 8),
    GameVersion(1, 8, 7),
    GameVersion(1, 8, 6),
    GameVersion(1, 8, 5),
    GameVersion(1, 8, 4),
    GameVersion(1, 8, 3),
    GameVersion(1, 8, 2),
    GameVersion(1, 8, 1),
    GameVersion(1, 8),
    GameVersion(1, 7, 10),
    GameVersion(1, 7, 9),
    GameVersion(1, 7, 8),
    GameVersion(1, 7, 7),
    GameVersion(1, 7, 6),
    GameVersion(1, 7, 5),
    GameVersion(1, 7, 4),
    GameVersion(1, 7, 3),
    GameVersion(1, 7, 2),
    GameVersion(1, 6, 4),
    GameVersion(1, 6, 2),
    GameVersion(1, 6, 1),
    GameVersion(1, 5, 2),
    GameVersion(1, 5, 1),
    GameVersion(1, 4, 7),
    GameVersion(1, 4, 6),
    GameVersion(1, 4, 5),
    GameVersion(1, 4, 4),
    GameVersion(1, 4, 2),
    GameVersion(1, 3, 2),
    GameVersion(1, 3, 1),
    GameVersion(1, 2, 5),
    GameVersion(1, 2, 4),
    GameVersion(1, 2, 3),
    GameVersion(1, 2, 2),
    GameVersion(1, 2, 1),
    GameVersion(1, 1),
    GameVersion(1, 0)
};

GameVersion::GameVersion(const QString &string) :
    versionString_(string)
{
    auto l = versionString_.split(".");
    if(l.size() >= 1)
        mainVersionNumber_ = l.at(0).toInt();
    if(l.size() >= 2)
        majorVersionNumber_ = l.at(1).toInt();
    if(l.size() == 3)
        minorVersionNumber_ = l.at(2).toInt();
}

GameVersion::GameVersion(int mainVersion, int majorVersion, int minorVersion) :
    mainVersionNumber_(mainVersion),
    majorVersionNumber_(majorVersion),
    minorVersionNumber_(minorVersion)
{
    QStringList list;
    list << QString::number(mainVersion) << QString::number(majorVersion);
    if(minorVersion)
        list << QString::number(minorVersion);
    versionString_ = list.join(".");
}

GameVersion GameVersion::majorVersion() const
{
    return GameVersion(mainVersionNumber_, majorVersionNumber_);
}

const QString &GameVersion::versionString() const
{
    return versionString_;
}

bool GameVersion::isDev() const
{
    return versionString_.contains('w') || versionString_.contains("snapshot", Qt::CaseInsensitive) ||
            versionString_.contains("pre") || versionString_.contains("rc") || versionString_.contains("rd");
}

GameVersion::operator QString() const
{
    return toString();
}

QString GameVersion::toString() const
{
    return versionString_.isEmpty()? QObject::tr("Any") : versionString_;
}

bool GameVersion::operator==(const GameVersion &other) const
{
    return versionString_ == other.versionString_;
}

bool GameVersion::operator!=(const GameVersion &another) const
{
    return versionString_ != another.versionString_;
}

GameVersion GameVersion::deduceFromString(const QString &string)
{
    QRegExp re(R"((\d+\.\d+(\.\d+)?))");
    if(re.indexIn(string) != -1){
        //2nd cap
        auto str = re.cap(1);
        if(mojangReleaseVersionList().contains(str))
            return {GameVersion(str)};
    }
    return GameVersion::Any;
}

void VersionManager::initMojangVersionList()
{
    auto accessManager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://launchermeta.mojang.com/mc/game/version_manifest.json"));
    auto reply = accessManager->get(request);
    QObject::connect(reply, &QNetworkReply::finished, this, [=]{
        if(reply->error() != QNetworkReply::NoError){
            qDebug() << reply->errorString();
            return;
        }
        QJsonParseError error;
        auto json = QJsonDocument::fromJson(reply->readAll(), &error);
        if(error.error == QJsonParseError::NoError){
            auto list = value(json.toVariant(), "versions").toList();
            for(const auto &entry : qAsConst(list)){
                GameVersion::mojangVersionList_ << value(entry, "id").toString();
                if(value(entry, "type").toString() == "release")
                    GameVersion::mojangReleaseVersionList_ << value(entry, "id").toString();
            }
        }
        qDebug() << "mojang version updated.";
        emit mojangVersionListUpdated();
        emit modrinthVersionListUpdated();
    });
}

void VersionManager::initCurseforgeVersionList()
{
    //get version list
    CurseforgeAPI::api()->getMinecraftVersionList([=](const auto &versionList){
        GameVersion::curseforgeVersionList_ = versionList;
        emit curseforgeVersionListUpdated();
        qDebug() << "curseforge version updated.";
    });
}

QList<GameVersion> GameVersion::mojangReleaseVersionList()
{
    return mojangVersionList_.isEmpty()? cachedVersionList_ : mojangReleaseVersionList_;
}

QList<GameVersion> GameVersion::curseforgeVersionList()
{
    return curseforgeVersionList_.isEmpty()? cachedVersionList_ : curseforgeVersionList_;
}

QList<GameVersion> GameVersion::modrinthVersionList()
{
    if(Config().getShowModrinthSnapshot())
        return mojangVersionList_.isEmpty()? cachedVersionList_ : mojangVersionList_;
    else
        return mojangVersionList_.isEmpty()? cachedVersionList_ : mojangReleaseVersionList_;
}

VersionManager *VersionManager::manager()
{
    static VersionManager manager;
    return &manager;
}

void VersionManager::initVersionLists()
{
    manager()->initMojangVersionList();
    manager()->initCurseforgeVersionList();
}
