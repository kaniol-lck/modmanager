#include "smoothscrollbar.h"

#include <QWheelEvent>

SmoothScrollBar::SmoothScrollBar(QWidget *parent) :
    QScrollBar(parent)
{
    timer_.setInterval(1000 / 60);
    connect(&timer_, &QTimer::timeout, this, &SmoothScrollBar::scollMove);
}

void SmoothScrollBar::wheelEvent(QWheelEvent *event)
{
    auto degree = event->angleDelta().y();
    if(value() == minimum() && degree > 0) return;
    if(value() == maximum() && degree < 0) return;
    speed_ -= degree * 0.03;
    timer_.start();
}

void SmoothScrollBar::scollMove()
{
    bool postive = speed_ > 0;
    int f = postive? -1 : 1;
    speed_ += f * 0.5;
    auto v = value() + speed_;
        setValue(v);
    if(v < minimum() + 0.001 || v > maximum() - 0.001)
        speed_ = 0;
    if((postive && speed_ <= 0) || (!postive && speed_ >= 0)){
        speed_ = 0;
        timer_.stop();
    }
}
