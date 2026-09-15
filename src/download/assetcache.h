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
    //下载失败（网络错误/超时/写文件失败）。
    //调用方需要它才能把「正在获取」的标记复位以便重试——否则一次失败会让该资源
    //永远卡在「正在获取」，图标再也不会上屏。
    void assetFailed();

private:
    QUrl url_;
    QString destFilePath_;
};

#endif // ASSETCACHE_H
