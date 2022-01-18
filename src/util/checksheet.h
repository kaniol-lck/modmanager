#ifndef CHECKSHEET_H
#define CHECKSHEET_H

#include <QObject>

class CheckSheet : public QObject
{
    Q_OBJECT
public:
    explicit CheckSheet(QObject *parent = nullptr);

    void start();
    void done();

    template <typename Func1, typename Func2>
    void add(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 startSignal, Func2 finishSignal)
    {
        startConnections_ << connect(sender, startSignal, this, [=]{
            finishConnections_ << connect(sender, finishSignal, this, &CheckSheet::onOneFinished);
        });
    }

    bool isWaiting() const;

    int failedCount() const;

public slots:
    void reset();

signals:
    void started();
    void progress(int doneCount, int count);
    void finished(bool success = true);

private slots:
    void onOneFinished(bool success = true);

private:
    QList<QMetaObject::Connection> startConnections_;
    QList<QMetaObject::Connection> finishConnections_;
    int finishedCount_ = 0;
    int failedCount_ = 0;
    bool isAdding_ = false;
};

#endif // CHECKSHEET_H
