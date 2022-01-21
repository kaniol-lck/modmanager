#ifndef CHECKSHEET_H
#define CHECKSHEET_H

#include <QMap>
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
            objects_.insert(sender, { false, false });
            finishConnections_ << connect(sender, finishSignal, [=](bool success){
                objects_[sender] = { true, success };
                if(isAdding_) return;
                emit progress(finishedCount(), objects_.count());
                if(allFinished()){
                    emit finished(!failedCount());
                    reset();
                }
            });
        });
    }

    bool isWaiting() const;

    QList<const QObject *> finishedObjects() const;
    int finishedCount() const;

    QList<const QObject *> unfinishedObjects() const;
    int unfinishedCount() const;

    QList<const QObject *> failedObjects() const;
    int failedCount() const;

public slots:
    void reset();

signals:
    void started();
    void progress(int doneCount, int count);
    void finished(bool success = true);

private:
    QList<QMetaObject::Connection> startConnections_;
    QList<QMetaObject::Connection> finishConnections_;
    enum { Finished, Success };
    QMap<const QObject *, std::tuple<bool, bool>> objects_;
    bool isAdding_ = false;

    bool allFinished() const;
};

#endif // CHECKSHEET_H
