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
        emit finished();
    } else if(finishedCount_ >= finishConnections_.count()){
//        qDebug() << "finished";
        reset();
        emit finished();
    }
}

bool CheckSheet::isWaiting() const
{
    return finishedCount_ < finishConnections_.count();
}

void CheckSheet::reset()
{
    for(auto &&conn : startConnections_){
        disconnect(conn);
    }
    for(auto &&conn : finishConnections_){
        disconnect(conn);
    }
    startConnections_.clear();
    finishConnections_.clear();
    finishedCount_ = 0;
}

void CheckSheet::cancel()
{
    reset();
}

void CheckSheet::onOneFinished()
{
    finishedCount_ ++;
    if(isAdding_) return;
    emit progress(finishedCount_, finishConnections_.count());
//    qDebug() << finishedCount_ << "/" << finishConnections_.count();
    if(finishedCount_ >= finishConnections_.count()){
//        qDebug() << "finished";
        reset();
        emit finished();
    }
}
