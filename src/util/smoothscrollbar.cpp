#include "smoothscrollbar.h"

#include <QWheelEvent>
#include <QDebug>

SmoothScrollBar::SmoothScrollBar(QWidget *parent) :
    QScrollBar(parent)
{
    accTimer_.setInterval(1000 / kRefreshRate);
    timer_.setInterval(1000 / kRefreshRate);
    connect(&accTimer_, &QTimer::timeout, this, &SmoothScrollBar::accSpeed);
    connect(&timer_, &QTimer::timeout, this, &SmoothScrollBar::scollMove);
}

void SmoothScrollBar::wheelEvent(QWheelEvent *event)
{
    if(config_.getSmoothScroll()){
        auto degree = event->angleDelta().y();
        if(value() == minimum() && degree > 0) return;
        if(value() == maximum() && degree < 0) return;
        if(abs(speed_) < 500)
            speed_ -= degree * 0.01 * config_.getScrollSpeed();
        if(abs(acc_) < 15)
            acc_ -= degree * 0.01;
        timer_.start();
        accTimer_.start();
    } else
        QScrollBar::wheelEvent(event);
}

void SmoothScrollBar::scollMove()
{
    bool postive = speed_ > 0;
    int f = postive? -1 : 1;
    speed_ += f * kRefreshRate * 0.01 * config_.getScrollFriction();
    auto v = value() + speed_;
        setValue(v);
    if(v < minimum() + 0.001 || v > maximum() - 0.001)
        speed_ = 0;
    if((postive && speed_ <= 0) || (!postive && speed_ >= 0)){
        speed_ = 0;
        timer_.stop();
    }
}

void SmoothScrollBar::accSpeed()
{
    speed_ += acc_ * kRefreshRate * 0.005 * config_.getScrollAcceleration();
    acc_ *= 0.9;
    if(std::abs(acc_) <= 0.001){
        acc_ = 0;
        accTimer_.stop();
    }
}
