#ifndef COMMONMODINFO_H
#define COMMONMODINFO_H

#include <QUrl>

class CommonModInfo
{
public:
    CommonModInfo() = default;

    const QString &id() const;
    const QString &name() const;
    const QString &version() const;
    const QStringList &authors() const;
    const QString &description() const;
    const QByteArray &iconBytes() const;
    const QUrl &homepage() const;
    const QUrl &sources() const;
    const QUrl &issues() const;
    const QStringList &languages() const;

protected:
    //basic
    QString id_;
    QString name_;
    QString version_;
    QStringList authors_;
    QString description_;
    QByteArray iconBytes_;
    QStringList languages_;

    //links
    QUrl homepage_;
    QUrl sources_;
    QUrl issues_;
};

#endif // COMMONMODINFO_H
