#include "webtextbrowser.h"

#include <QRegularExpression>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QBuffer>

#include "util/smoothscrollbar.h"

WebTextBrowser::WebTextBrowser(QWidget *parent) :
    QTextBrowser(parent)
{
    setVerticalScrollBar(new SmoothScrollBar(this));
}

void WebTextBrowser::setHtml(const QString &html)
{
    for(auto &&reply : qAsConst(replies_)){
        if(!reply->isFinished()){
            reply->abort();
            reply->deleteLater();
        }
    }
    replies_.clear();

    QTextBrowser::setHtml(html);
    static QRegularExpression reg(R"_(<img .*?src="(.*?)".*?>)_");
    for(auto i = reg.globalMatch(html); i.hasNext();){
        auto match = i.next();
        QString imageStr = match.captured(1);
        QUrl imageUrl(imageStr);

        QNetworkRequest request(imageUrl);
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);

        static QNetworkAccessManager accessManager;
        auto reply = accessManager.get(request);
        replies_ << reply;
        connect(reply, &QNetworkReply::finished, this, [=]{
            if(reply->error() != QNetworkReply::NoError) return;
            QPixmap pixmap;
            pixmap.loadFromData(reply->readAll());

            QByteArray byteArray;
            QBuffer buffer(&byteArray);
            pixmap.toImage().save(&buffer, "JPEG");
            QString base64Img = "data:image/png;base64," + QString::fromLatin1(byteArray.toBase64().data());
            QTextBrowser::setHtml(QTextBrowser::toHtml().replace(imageUrl.toString(), base64Img));
            replies_.removeOne(reply);
            reply->deleteLater();
        });
    }
}
