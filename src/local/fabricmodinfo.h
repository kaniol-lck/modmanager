#ifndef FABRICMODINFO_H
#define FABRICMODINFO_H

#include <QList>
#include <QMap>

class QuaZip;
class LocalMod;

class FabricModInfo
{
public:
    FabricModInfo() = default;
    static QList<FabricModInfo> fromZip(const QString &path);
    static QList<FabricModInfo> fromZip(QuaZip *zip, const QString &mainId = "");

    const QString &id() const;
    const QString &name() const;
    const QString &version() const;
    const QStringList &authors() const;
    const QString &description() const;
    const QByteArray &iconBytes() const;
    const QStringList &provides() const;
    const QMap<QString, QString> &depends() const;
    const QMap<QString, QString> &conflicts() const;
    const QMap<QString, QString> &breaks() const;

    bool isEmbedded() const;
    void setIsEmbedded(bool newIsEmbedded);

    LocalMod *localMod() const;
    void setLocalMod(LocalMod *newLocalMod);

    const QString &path() const;

    const QString &mainId() const;

private:
    QString id_;
    QString name_;
    QString version_;
    QStringList authors_;
    QString description_;
    QByteArray iconBytes_;
    QStringList provides_;
    QMap<QString, QString> depends_;
    QMap<QString, QString> conflicts_;
    QMap<QString, QString> breaks_;

    bool isEmbedded_ = true;
    //to mark where comes from
    QString mainId_;
};

#endif // FABRICMODINFO_H
