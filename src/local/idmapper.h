#ifndef IDMAPPER_H
#define IDMAPPER_H

#include <QMap>
#include <QJsonObject>

class IdMapper
{
    //TODO: what if fabric and forge has same modid  and different website id;
    class Id
    {
        friend class IdMapper;
    public:
        Id() = default;
        explicit Id(const QString &modid);
        static Id fromJsonObject(const QString &modid, const QVariant &variant);
        QJsonObject toJsonObject() const;
        const QString &modid() const;
        int curseforgeId() const;
        const QString &modrinthId() const;
    private:
        QString modid_;
        int curseforgeId_;
        QString modrinthId_;
    };
public:
    IdMapper();
    static IdMapper *mapper();
    static const QMap<QString, Id> &idMap();
    static Id get(const QString &modid);
    static void addCurseforge(const QString &modid, int curseforgeId);
    static void addModrinth(const QString &modid, QString modrinthId);
private:
    void writeToFile();
    void readFromFile();

    static constexpr auto kFileName = "modid.json";
    QMap<QString, Id> idMap_;
};

#endif // IDMAPPER_H
