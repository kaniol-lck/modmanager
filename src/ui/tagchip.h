#ifndef TAGCHIP_H
#define TAGCHIP_H

#include <QString>

class QLabel;
class QWidget;
class Tag;

// 标签芯片（tag chip）：mod 条目、信息页、筛选器里那些彩色小圆角标签。
// 外观只在这里定义一次，TagsWidget / TagsFlowWidget 共用 —— 否则两处复制同一段
// 样式字符串，改一处漏一处。
//
// 量出来的两组数（离屏探针 .workbuddy/probe/tagshot，10pt 微软雅黑）：
//   改前 mod 条目里芯片 51×41，左右墨迹留白各 7/8px，上下却有 16/16px
//        —— QHBoxLayout 把 QLabel 纵向拉满整行，半径 10 的"药丸"变成方块，横向反而贴边；
//   改后 51×22 药丸，左右各 11px，上下各 6px。
// 所以芯片中心的那两行（固定高度 + padding）不是随便取的，别只改一半。
namespace TagChip {

// rich text 里图标的基准边长（px）
constexpr int kIconSize = 16;
// 芯片内边距：上下 3px、左右 8px。左右必须明显大于上下 ——
// 药丸两端是半圆，文字贴到弧线附近就会显挤。
constexpr int kPadY = 3;
constexpr int kPadX = 8;

// 芯片内联样式：白字 + 分类色底 + 药丸圆角（半径取高度一半）
QString styleSheet(const Tag &tag, int chipHeight);

// 造一个芯片。文本 = 图标（有则加）+ 名称；tooltip = "分类: 名称"。
// iconOnly 时若标签带图标则只显示图标（且不加底色，供 mod 列表紧凑排版用）。
// trContext 与原调用方的类名保持一致，避免翻译上下文漂移。
//
// 返回的 label 已按内容定死宽高（QSizePolicy::Fixed），调用方放进布局时
// 请用 Qt::AlignVCenter —— 否则在 QHBoxLayout 里会被纵向拉伸。
QLabel *create(const Tag &tag, QWidget *parent, bool iconOnly = false, const char *trContext = "TagsWidget");

} // namespace TagChip

#endif // TAGCHIP_H
