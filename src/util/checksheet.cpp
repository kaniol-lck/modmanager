#include "checksheet.h"

#include <QDebug>

CheckSheet::CheckSheet(QObject *parent) : QObject(parent)
{}

void CheckSheet::start()
{
    isAdding_ = true;
//    qDebug() << "started";
    emit started();
}

void CheckSheet::done()
{
    isAdding_ = false;
//    qDebug() << startConnections_.size() << finishConnections_.size();
    if(startConnections_.isEmpty()){
//        qDebug() << "finished: nothing to do";
        emit finished(!failedCount());
    } else if(allFinished()){
//        qDebug() << "finished";
        emit finished(!failedCount());
        reset();
    }
}

bool CheckSheet::isWaiting() const
{
    return isAdding_ || !allFinished();
}

QList<const QObject *> CheckSheet::finishedObjects() const
{
    QList<const QObject *> list;
    for(auto it = objects_.begin(); it != objects_.end(); it ++)
        if(std::get<Finished>(*it)) list << it.key();
    return list;
}

int CheckSheet::finishedCount() const
{
    return finishedObjects().count();
}

QList<const QObject *> CheckSheet::unfinishedObjects() const
{
    QList<const QObject *> list;
    for(auto it = objects_.begin(); it != objects_.end(); it ++)
        if(!std::get<Finished>(*it)) list << it.key();
    return list;
}

int CheckSheet::unfinishedCount() const
{
    return unfinishedObjects().size();
}

QList<const QObject *> CheckSheet::failedObjects() const
{
    QList<const QObject *> list;
    for(auto it = objects_.begin(); it != objects_.end(); it ++)
        if(!std::get<Success>(*it)) list << it.key();
    return list;
}

void CheckSheet::reset()
{
    for(auto &&conn : startConnections_){
        if(conn) disconnect(conn);
    }
    for(auto &&conn : finishConnections_){
        disconnect(conn);
    }
    startConnections_.clear();
    finishConnections_.clear();
    objects_.clear();
}

bool CheckSheet::allFinished() const
{
    for(auto &&[finished, success] : objects_)
        if(!finished) return false;
    return true;
}

int CheckSheet::failedCount() const
{
    return failedObjects().size();
}
