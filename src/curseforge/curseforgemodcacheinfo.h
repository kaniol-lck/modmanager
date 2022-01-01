#ifndef CURSEFORGEMODCACHEINFO_H
#define CURSEFORGEMODCACHEINFO_H

#include <QPixmap>
#include <QUrl>
#include <QVariant>

class QJsonObject;
class CurseforgeModCacheInfo
{
public:
    static const QString &cachePath();

    CurseforgeModCacheInfo() = default;
    explicit CurseforgeModCacheInfo(int addonId);
    static CurseforgeModCacheInfo fromVariant(const QVariant &variant);
    QJsonObject toJsonObject() const;

    bool operator==(const CurseforgeModCacheInfo &other) const;

    int id() const;
    const QString &name() const;
    const QString &summary() const;
    const QString &slug() const;
    const QUrl &iconUrl() const;
    const QPixmap &icon() const;
    double popularityScore() const;

protected:
    int id_;
    QString name_;
    QString summary_;
    QString slug_;
    QUrl iconUrl_;
    QPixmap icon_;
    double popularityScore_;
};


#endif // CURSEFORGEMODCACHEINFO_H
