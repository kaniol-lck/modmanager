#include "speedrecorder.h"

#include <QDateTime>

SpeedRecorder::SpeedRecorder(QObject *parent)
    : QObject{parent}
{
    connect(this, &SpeedRecorder::downloadSpeed, [=](qint64 download, qint64 upload){
        downSpeed_ = download;
        upSpeed_ = upload;
    });
    connect(this, &SpeedRecorder::finished, [=]{
        downSpeed_ = 0;
        upSpeed_ = 0;
    });
    //update speed point every 1s
    timer_.start(1000);
    connect(&timer_, &QTimer::timeout, this, [=]{
            addData(downSpeed_, upSpeed_);
    });
}

void SpeedRecorder::addData(qint64 downSpeed, qint64 upSpeed)
{
    PointData point;
    point.x = QDateTime::currentMSecsSinceEpoch() / 1000;
    point.y[Download] = downSpeed;
    point.y[Upload] = upSpeed;

    dataCollection_.push_back(point);

    while (dataCollection_.length() > DATA_MAXSIZE)
        dataCollection_.removeFirst();

    emit dataUpdated();
}

qint64 SpeedRecorder::downSpeed() const
{
    return downSpeed_;
}

qint64 SpeedRecorder::upSpeed() const
{
    return upSpeed_;
}

const QList<SpeedRecorder::PointData> &SpeedRecorder::dataCollection() const
{
    return dataCollection_;
}
