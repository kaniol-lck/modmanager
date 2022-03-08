#ifndef DOWNLOADERSPEEDWIDGET_H
#define DOWNLOADERSPEEDWIDGET_H

#include <QGraphicsView>
#include <QMap>
#include <QPen>

#include "download/speedrecorder.h"

// Modified from SpeedWidget in Qv2ray
class DownloaderSpeedWidget : public QGraphicsView
{
    Q_OBJECT
public:
    explicit DownloaderSpeedWidget(QWidget *parent = nullptr);

    void clear();
    void replot();


    SpeedRecorder *speedrecorder() const;
    void setSpeedRecorder(SpeedRecorder *newSpeedRecorder);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    qint64 maxYValue();
    SpeedRecorder *speedRecorder_ = nullptr;
    QList<QPair<QString, QPen>> pens_;
};

#endif // DOWNLOADERSPEEDWIDGET_H
