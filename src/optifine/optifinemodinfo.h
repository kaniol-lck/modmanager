#ifndef OPTIFINEMODINFO_H
#define OPTIFINEMODINFO_H

#include <QUrl>

class OptifineMod;

class OptifineModInfo
{
    friend class OptifineMod;
public:
    OptifineModInfo() = default;
    static OptifineModInfo fromHtml(const QString &html);

    const QString &name() const;
    const QString &fileName() const;
    const QUrl &mirrorUrl() const;
    const QUrl &downloadUrl() const;

private:
    QString name_;
    QString fileName_;
    QUrl mirrorUrl_;
    QUrl downloadUrl_;
};

#endif // OPTIFINEMODINFO_H
