#ifndef DOWNLOADERSPEEDWIDGET_H
#define DOWNLOADERSPEEDWIDGET_H

#include <QGraphicsView>
#include <QMap>
#include <QPen>
#include <QTimer>

#include "download/abstractdownloader.h"

// Modified from SpeedWidget in Qv2ray
class DownloaderSpeedWidget : public QGraphicsView
{
    Q_OBJECT
public:
    explicit DownloaderSpeedWidget(QWidget *parent = nullptr);

    void clear();
    void replot();

    AbstractDownloader *downloader() const;
    void setDownloader(AbstractDownloader *newDownloader);

private slots:
    void addSpeedPoint(qint64 downSpeed, qint64 upSpeed);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    qint64 maxYValue();
    QTimer timer_;
    AbstractDownloader *downloader_ = nullptr;
    QList<QPair<QString, QPen>> pens_;

    static constexpr auto VIEWABLE = 120;
};

#endif // DOWNLOADERSPEEDWIDGET_H
