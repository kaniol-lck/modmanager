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

    const QUrl &getWebsiteUrl() const;

    const QStringList &getAuthors() const;

    void setDescription(const QString &newDescription);

    const QString &getDescription() const;

signals:
    void thumbnailReady();

private slots:
    void thumbnailDownloadFinished(QNetworkReply* reply);

private:
    int id;
    QString name;
    QString summary;
    QUrl websiteUrl;
    QStringList authors;
    QByteArray thumbnailBytes;
    QString description;

    QNetworkAccessManager *accessManager;

};

#endif // CURSEFORGEMOD_H
