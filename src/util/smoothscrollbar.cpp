#include "smoothscrollbar.h"

#include <QWheelEvent>
#include <QDebug>

SmoothScrollBar::SmoothScrollBar(QWidget *parent) :
    QScrollBar(parent)
{
    timer_.setInterval(1000 / kRefreshRate);
    connect(&timer_, &QTimer::timeout, this, &SmoothScrollBar::scollMove);
    //用户直接拖滚动条时立刻让位，不要和动画抢同一个值
    connect(this, &QScrollBar::sliderPressed, this, [this]{
        timer_.stop();
        target_ = expectedValue_ = value();
    });
}

void SmoothScrollBar::wheelEvent(QWheelEvent *event)
{
    if(!config_.getSmoothScroll()){
        QScrollBar::wheelEvent(event);
        return;
    }

    //关键：位移量直接采用 Qt 自己算出来的结果，不去猜刻度语义/设备类型/DPI/滚动相位。
    //先让基类按原生规则改一次值，读出它走了多少，再撤回，改为由动画平滑趋近。
    //这样「一次滚轮走多远」与原生完全一致，只是把这一步摊成一小段动画。
    const int before = value();
    QScrollBar::wheelEvent(event);
    const int delta = value() - before;
    if(delta == 0) return;              //到边界了，原生行为就是不动

    setValue(before);                    //撤回基类的直接跳变（同一事件内，不会闪）
    expectedValue_ = before;

    //scrollSpeed：一次滚轮相对原生的距离倍率（1.0 = 与原生完全一致）
    const double travel = delta * config_.getScrollSpeed();
    //scrollFriction：摩擦越大 ⇒ 每帧追赶比例越大 ⇒ 滑行越短（1.0 = 基准值）。
    //在这里解析一次并缓存，避免计时器回调里每帧读一次 QSettings。
    followRatio_ = qBound(kMinFollowRatio,
                          kBaseFollowRatio * config_.getScrollFriction(),
                          kMaxFollowRatio);

    if(!timer_.isActive()){
        target_ = before;
        timer_.start();
    }
    //动画在跑就只推进目标：新滚轮不会与旧动画互相抵消，所以不会「不响应」
    target_ = qBound<double>(minimum(), target_ + travel, maximum());

    //scrollAcceleration：起手立刻走掉一部分，让手感更跟手。
    //1.0 时不做拆分（保持既有手感），越大立即位移越多。
    const double immediateRatio = qBound(0.0,
                                         (config_.getScrollAcceleration() - 1.0) * 0.5,
                                         kMaxImmediateRatio);
    if(immediateRatio > 0.0){
        const int step = qRound((target_ - value()) * immediateRatio);
        if(step != 0){
            const int next = qBound(minimum(), value() + step, maximum());
            setValue(next);
            expectedValue_ = next;
        }
    }
}

void SmoothScrollBar::scollMove()
{
    const int v = value();
    //值被外部改动（列表重载、程序设置等）：立刻让位，避免朝过期目标乱滚
    if(v != expectedValue_){
        timer_.stop();
        target_ = expectedValue_ = v;
        return;
    }

    const double diff = target_ - v;
    if(qAbs(diff) <= 1.0){
        //足够近就吸附到目标并停止。这个分支保证一定收敛。
        const int last = qRound(target_);
        if(last != v) setValue(last);
        expectedValue_ = last;
        timer_.stop();
        return;
    }

    //每帧走剩余距离的固定比例；至少 1 像素，否则 diff 很小时会一直空转不收敛
    const int step = qMax(1, int(qAbs(diff) * followRatio_));
    const int next = v + (diff > 0 ? step : -step);
    setValue(next);
    expectedValue_ = next;
}
