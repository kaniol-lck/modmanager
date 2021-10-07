#ifndef SMOOTHSCROLLBAR_H
#define SMOOTHSCROLLBAR_H

#include <QScrollBar>

class QPropertyAnimation;

class SmoothScrollBar : public QScrollBar
{
public:
    SmoothScrollBar(QWidget *parent);

protected:
    virtual void wheelEvent(QWheelEvent *event);

private:
    QPropertyAnimation *animation_;
    static constexpr int kTime = 300;
};

#endif // SMOOTHSCROLLBAR_H
