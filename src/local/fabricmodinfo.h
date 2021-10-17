#ifndef FABRICMODINFO_H
#define FABRICMODINFO_H

#include "commonmodinfo.h"

#include <QMap>
class QuaZip;

class FabricModInfo : public CommonModInfo
{
public:
    FabricModInfo() = default;
    static QList<FabricModInfo> fromZip(const QString &path);
    static QList<FabricModInfo> fromZip(QuaZip *zip, const QString &mainId = "");

    const QStringList &provides() const;
    const QMap<QString, QString> &depends() const;
    const QMap<QString, QString> &conflicts() const;
    const QMap<QString, QString> &breaks() const;

    const QString &mainId() const;

    bool isEmbedded() const;
    void setIsEmbedded(bool newIsEmbedded);

    const QString &environment() const;

private:
    QStringList provides_;
    QMap<QString, QString> depends_;
    QMap<QString, QString> conflicts_;
    QMap<QString, QString> breaks_;
    QString environment_;

    bool isEmbedded_ = true;
    //to mark where comes from
    QString mainId_;
};

#endif // FABRICMODINFO_H
