#ifndef SMOOTHSCROLLBAR_H
#define SMOOTHSCROLLBAR_H

#include <QScrollBar>
#include <QTimer>

#include "config.hpp"

class QPropertyAnimation;

class SmoothScrollBar : public QScrollBar
{
public:
    SmoothScrollBar(QWidget *parent);
protected:
    virtual void wheelEvent(QWheelEvent *event);
private slots:
    void scollMove();
    void accSpeed();
private:
    Config config_;
    QTimer accTimer_;
    QTimer timer_;
    static constexpr int kRefreshRate = 75;
    double acc_ = 0;
    double speed_ = 0;
    static constexpr int kTime = 300;
};

#endif // SMOOTHSCROLLBAR_H
