#ifndef CURSEFORGEMOD_H
#define CURSEFORGEMOD_H

#include <QObject>
#include <QVariant>
#include <QUrl>

class QNetworkReply;
class QNetworkAccessManager;

class CurseforgeMod : public QObject
{
    Q_OBJECT
public:
    CurseforgeMod(QObject *parent);

    static CurseforgeMod *fromFingerprint(QObject *parent, QString murmurhash);
    static CurseforgeMod *fromVariant(QObject *parent, QVariant variant);

    void downloadThumbnail();

    int getId() const;

    const QString &getName() const;

    const QString &getSummary() const;

    const QByteArray &getThumbnailBytes() const;

    const QUrl &getWebsiteUrl() const;

    const QStringList &getAuthors() const;

    void setDescription(const QString &newDescription);

    const QString &getDescription() const;

    const QUrl &getLatestFileUrl() const;

    const QString &getLatestFileName() const;

    int getLatestFileLength() const;

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
    QUrl thumbnailUrl;
    QByteArray thumbnailBytes;
    QUrl latestFileUrl;
    QString latestFileName;
    int latestFileLength;
    QString description;

    QNetworkAccessManager *accessManager;

};

#endif // CURSEFORGEMOD_H
