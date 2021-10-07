#include "smoothscrollbar.h"

#include <QPropertyAnimation>
#include <QWheelEvent>
#include <QDebug>

SmoothScrollBar::SmoothScrollBar(QWidget *parent) :
    QScrollBar(parent),
    animation_(new QPropertyAnimation(this, "value"))
{
    animation_->setDuration(kTime);
    animation_->setEasingCurve(QEasingCurve::InOutQuad);
}

void SmoothScrollBar::wheelEvent(QWheelEvent *event)
{
    auto degree = event->angleDelta().y() * 2;
    if(value() == minimum() && degree > 0) return;
    if(value() == maximum() && degree < 0) return;
    if(animation_->state() == QAbstractAnimation::Stopped){
        animation_->setEndValue(value() - degree);
        animation_->start();
    } else if(animation_->state() == QAbstractAnimation::Running){
        animation_->stop();
        animation_->setEndValue(animation_->endValue().toInt() - degree);
        animation_->start();
    }
}
