#ifndef SMOOTHSCROLLBAR_H
#define SMOOTHSCROLLBAR_H

#include <QScrollBar>
#include <QTimer>

class QPropertyAnimation;

class SmoothScrollBar : public QScrollBar
{
public:
    SmoothScrollBar(QWidget *parent);
protected:
    virtual void wheelEvent(QWheelEvent *event);
private slots:
    void scollMove();
private:
    QTimer timer_;
    double speed_ = 0;
    static constexpr int kTime = 300;
};

#endif // SMOOTHSCROLLBAR_H
