#ifndef YOUDAOTRANSLATOR_H
#define YOUDAOTRANSLATOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>

class YoudaoTranslator : public QObject
{
    Q_OBJECT
public:
    explicit YoudaoTranslator(QObject *parent = nullptr);
    static YoudaoTranslator *translator();

    void translate(const QString &str, std::function<void (QString)> callback);
private:
    QNetworkAccessManager accessManager_;
    QNetworkDiskCache diskCache_;
};

#endif // YOUDAOTRANSLATOR_H
