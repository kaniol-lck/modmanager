#ifndef WEBSITEICON_H
#define WEBSITEICON_H

#include <QObject>
#include <QIcon>

class WebsiteIcon : public QObject
{
    Q_OBJECT
public:
    explicit WebsiteIcon(QObject *parent = nullptr);

    void get(QUrl websiteUrl);

signals:
    void iconGot(QIcon icon);

private:
};

#endif // WEBSITEICON_H
