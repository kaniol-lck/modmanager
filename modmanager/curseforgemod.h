#ifndef CURSEFORGEMOD_H
#define CURSEFORGEMOD_H

#include <QObject>
#include <QVariant>
#include <QNetworkAccessManager>

class CurseforgeMod : public QObject
{
    Q_OBJECT
public:
    CurseforgeMod(QObject *parent);

    static CurseforgeMod *fromFingerprint(QObject *parent, QString murmurhash);
    static CurseforgeMod *fromVariant(QObject *parent, QVariant variant);

    int getId() const;

    const QString &getName() const;

    const QString &getSummary() const;

    const QByteArray &getThumbnailBytes() const;

signals:
    void thumbnailReady();

private slots:
    void thumbnailDownloadFinished(QNetworkReply* reply);

private:
    int id;
    QString name;
    QString summary;
    QByteArray thumbnailBytes;

    QNetworkAccessManager *accessManager;

};

#endif // CURSEFORGEMOD_H
