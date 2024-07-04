#ifndef WEBTEXTBROWSER_H
#define WEBTEXTBROWSER_H

#include <QTextBrowser>

class QNetworkReply;
class WebTextBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    explicit WebTextBrowser(QWidget *parent = nullptr);

    void setHtml(const QString &html);
private:
    QList<QNetworkReply*> replies_;
};

#endif // WEBTEXTBROWSER_H
