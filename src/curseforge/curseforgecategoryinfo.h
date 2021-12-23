#ifndef CURSEFORGECATEGORYINFO_H
#define CURSEFORGECATEGORYINFO_H

#include <QDateTime>
#include <QString>
#include <QUrl>
#include <QVariant>

class CurseforgeCategoryInfo
{
public:
    CurseforgeCategoryInfo() = default;
    static CurseforgeCategoryInfo fromVariant(const QVariant &variant);

    int id() const;
    const QString &name() const;
    const QString &slug() const;
    const QUrl &avatarUrl() const;
    const QDateTime &dateModified() const;
    int parentGameCategoryId() const;
    int rootGameCategoryId() const;

private:
    int id_;
    QString name_;
    QString slug_;
    QUrl avatarUrl_;
    QDateTime dateModified_;
    int parentGameCategoryId_;
    int rootGameCategoryId_;
};

#endif // CURSEFORGECATEGORYINFO_H
