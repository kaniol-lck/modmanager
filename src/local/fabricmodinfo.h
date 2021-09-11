#ifndef FABRICMODINFO_H
#define FABRICMODINFO_H

#include <QList>
#include <QMap>

class QuaZip;

class FabricModInfo
{
public:
    FabricModInfo() = default;

    static QList<FabricModInfo> fromZip(QuaZip *zip);

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
};

#endif // FABRICMODINFO_H
