#include "tagchip.h"

#include <QCoreApplication>
#include <QFontMetrics>
#include <QLabel>
#include <QSizePolicy>

#include "tag/tag.h"

namespace TagChip {

QString styleSheet(const Tag &tag, int chipHeight)
{
    // 竖直方向不写 padding：高度已经由 create() 定死，文字由 QLabel 默认的
    // AlignVCenter 居中；两边都写会打架（padding 把内容压扁，居中再补一次）。
    return QStringLiteral("color: #fff; background-color: %1; border-radius: %2px; padding: 0 %3px;")
        .arg(tag.category().color().name())
        .arg(chipHeight / 2)   // 半径取高的一半 = 正圆端，高度变了也还是药丸
        .arg(kPadX);
}

QLabel *create(const Tag &tag, QWidget *parent, bool iconOnly, const char *trContext)
{
    auto label = new QLabel(parent);

    // 图标与文字同时要放进标签时，图标相对文字基线的位置由 vertical-align 决定。
    // 默认是 baseline：16px 的图标整块坐在基线上，视觉上比文字明显偏高。
    const QString icon = QStringLiteral(R"(<img src="%1" height="%2" width="%2" style="vertical-align: middle"/>)")
                             .arg(tag.iconName())
                             .arg(kIconSize);
    const bool hasIcon = !tag.iconName().isEmpty();
    if(hasIcon && iconOnly)
        label->setText(icon);
    else if(hasIcon)
        label->setText(icon + QStringLiteral(" ") + tag.name());
    else
        label->setText(tag.name());
    label->setToolTip(QCoreApplication::translate(trContext, "%1: %2").arg(tag.category().name(), tag.name()));

    // 高度按"图标与文字里更高的那个 + 上下内边距"定死：
    // 不能靠布局给 —— QHBoxLayout 会把 QLabel 纵向拉满整行（实测 41px），
    // 药丸就变成方块、文字贴着左右圆角，正是"标签拥挤"的来源。
    // 顺带让带图标/不带图标的芯片高度一致，同一行不会参差。
    const int contentH = qMax(kIconSize, label->fontMetrics().height());
    const int chipHeight = contentH + 2 * kPadY;
    label->setFixedHeight(chipHeight);
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    if(!iconOnly || !hasIcon)
        label->setStyleSheet(styleSheet(tag, chipHeight));
    return label;
}

} // namespace TagChip
