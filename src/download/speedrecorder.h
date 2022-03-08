#ifndef SPEEDRECORDER_H
#define SPEEDRECORDER_H

#include <QObject>
#include <QTimer>

class SpeedRecorder : public QObject
{
    Q_OBJECT
public:
    explicit SpeedRecorder(QObject *parent = nullptr);

    enum LineType { Download, Upload };
    struct PointData
    {
        qint64 x;
        qint64 y[2];
    };

    static constexpr auto DATA_MAXSIZE = 60;

    const QList<PointData> &dataCollection() const;
    qint64 downSpeed() const;
    qint64 upSpeed() const;

signals:
    void downloadSpeed(qint64 download, qint64 upload = 0);
    void finished();

    void dataUpdated();

private:
    void addData(qint64 downSpeed, qint64 upSpeed);

    QTimer timer_;
    QList<PointData> dataCollection_;
    qint64 downSpeed_ = 0;
    qint64 upSpeed_ = 0;
};

#endif // SPEEDRECORDER_H
