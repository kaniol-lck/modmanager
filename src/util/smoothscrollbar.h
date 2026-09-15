#ifndef SMOOTHSCROLLBAR_H
#define SMOOTHSCROLLBAR_H

#include <QScrollBar>
#include <QTimer>

#include "config.hpp"

class SmoothScrollBar : public QScrollBar
{
public:
    SmoothScrollBar(QWidget *parent);
protected:
    virtual void wheelEvent(QWheelEvent *event) override;
private slots:
    void scollMove();
private:
    Config config_;
    QTimer timer_;
    static constexpr int kRefreshRate = 60;
    //追赶比例基准（scrollFriction = 1 时的取值）。0.25 时约 16 帧（≈270ms）收敛，
    //且第一帧的步长最大 ⇒ 起手跟手、收尾自然。
    static constexpr double kBaseFollowRatio = 0.25;
    static constexpr double kMinFollowRatio = 0.05;
    static constexpr double kMaxFollowRatio = 0.9;
    //「起手立刻走掉」的比例上限，只在 scrollAcceleration > 1 时启用
    static constexpr double kMaxImmediateRatio = 0.8;

    //滚轮只负责推进 target_，实际值由动画指数趋近它。
    //把「目标」和「当前位置」拆成两个独立量，是为了让新滚轮只推进目标、
    //不可能与正在进行的动画互相抵消——这正是之前「有时不响应」的根源。
    double target_ = 0;
    //上一次由本类写入的值，用来识别「值被外部改动」（列表重载、程序设置等）
    int expectedValue_ = 0;
    //本次动画使用的追赶比例。在 wheelEvent 里按 scrollFriction 解析一次，
    //避免在计时器回调里每帧读一次 QSettings。
    double followRatio_ = kBaseFollowRatio;
};

#endif // SMOOTHSCROLLBAR_H
