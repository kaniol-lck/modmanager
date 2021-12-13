#include "checksheet.h"

CheckSheet::CheckSheet(QObject *parent) : QObject(parent)
{}

bool CheckSheet::isWaiting()
{
    return !startConnections_.isEmpty();
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
    emit progress(finishedCount_, finishConnections_.count());
    if(finishedCount_ >= finishConnections_.count()){
        reset();
        emit finished();
    }
}
