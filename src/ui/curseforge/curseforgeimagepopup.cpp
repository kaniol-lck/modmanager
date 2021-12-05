#include "curseforgeimagepopup.h"
#include "ui_curseforgeimagepopup.h"

#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QMouseEvent>
#include <QScreen>

CurseforgeImagePopup::CurseforgeImagePopup(QWidget *parent, const QUrl &url, const QString &title, const QString &desciption, const QByteArray &thumbnail) :
    QDialog(parent),
    ui(new Ui::CurseforgeImagePopup)
{
    ui->setupUi(this);
    setMaximumSize(screen()->size());
    setWindowFlags(Qt::Popup);
    setAttribute(Qt::WA_DeleteOnClose);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(63, 63, 63, 192));
    setPalette(palette);

    ui->title->setText(title);
    ui->description->setText(desciption);
    QPixmap pixmap;
    pixmap.loadFromData(thumbnail);
    ui->image->setPixmap(pixmap);

    QNetworkRequest request(url);
    static QNetworkAccessManager accessManager;
    static QNetworkDiskCache diskCache;
    diskCache.setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    accessManager.setCache(&diskCache);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    auto reply = accessManager.get(request);
    connect(reply, &QNetworkReply::finished, this, [=]{
        if(reply->error() != QNetworkReply::NoError) return;
        QPixmap pixmap;
        pixmap.loadFromData(reply->readAll());
        ui->image->setPixmap(pixmap);
        reply->deleteLater();
    });
}

CurseforgeImagePopup::~CurseforgeImagePopup()
{
    delete ui;
}
void CurseforgeImagePopup::mouseMoveEvent(QMouseEvent *event)
{
    QDialog::mouseMoveEvent(event);
    if(event->buttons() & Qt::LeftButton )
        move(event->pos() + pos() - clickPos_);
}

void CurseforgeImagePopup::mousePressEvent(QMouseEvent *event)
{
    QDialog::mousePressEvent(event);
    if(event->button() == Qt::LeftButton)
        clickPos_ = event->pos();
    if(event->button() == Qt::MiddleButton)
        close();
}

void CurseforgeImagePopup::mouseDoubleClickEvent(QMouseEvent *event[[maybe_unused]])
{
    if(geometry().size() == screen()->geometry().size()){
        resize(0, 0);
        move(pos() + event->pos() - QPoint(width(), height()) / 2);
    }
    else
        setGeometry(screen()->geometry());
}
