#ifndef ASSETCACHE_H
#define ASSETCACHE_H

#include <QObject>
#include <QUrl>

class AssetCache : public QObject
{
    Q_OBJECT
public:
    AssetCache(QObject *parent, const QUrl &url, const QString &destFilePath);
    AssetCache(QObject *parent, const QUrl &url, const QString &fileName, const QString &path);

    bool exists() const;
    void download();

    const QString &destFilePath() const;

signals:
    void assetReady();

private:
    QUrl url_;
    QString destFilePath_;
};

#endif // ASSETCACHE_H
