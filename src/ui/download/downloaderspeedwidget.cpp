#include "downloaderspeedwidget.h"

#include <QDateTime>
#include <QPainter>
#include <QLocale>

// table of supposed nice steps for grid marks to get nice looking quarters of scale
const static double roundingTable[] = { 1.2, 1.6, 2, 2.4, 2.8, 3.2, 4, 6, 8 };

DownloaderSpeedWidget::DownloaderSpeedWidget(QWidget *parent) : QGraphicsView(parent)
{
    pens_ << qMakePair(tr("Download"), QPen(QColor(0, 0, 255)));
    pens_ << qMakePair(tr("Upload"), QPen(QColor(0, 255, 0)));
    for(auto &[name, pen] : pens_){
        pen.setStyle(Qt::SolidLine);
        pen.setWidthF(1);
    }
}

// use binary prefix standards from IEC 60027-2
// see http://en.wikipedia.org/wiki/Kilobyte
enum SizeUnit : int
{
    Byte,  // 1000^0,
    KByte, // 1000^1,
    MByte, // 1000^2,
    GByte, // 1000^3,
    TByte, // 1000^4,
    PByte, // 1000^5,
    EByte  // 1000^6
};

QString unitString(const SizeUnit unit, const bool isSpeed)
{
    const static QStringList units{ "B", "KB", "MB", "GB", "TB", "PB", "EB" };
    auto unitString = units[unit];
    if (isSpeed)
        unitString += "/s";
    return unitString;
}

int friendlyUnitPrecision(const SizeUnit unit)
{
    // friendlyUnit's number of digits after the decimal point
    switch (unit)
    {
        case SizeUnit::Byte: return 0;
        case SizeUnit::KByte:
        case SizeUnit::MByte: return 1;
        case SizeUnit::GByte: return 2;
        default: return 3;
    }
}
struct SplittedValue
{
    double arg;
    SizeUnit unit;
    qint64 sizeInBytes() const
    {
        auto size = arg;
        for (int i = 0; i < static_cast<int>(unit); ++i)
        {
            size *= 1024;
        }
        return size;
    }
};

SplittedValue getRoundedYScale(double value)
{
    if (value == 0.0)
        return { 0, SizeUnit::Byte };

    if (value <= 12.0)
        return { 12, SizeUnit::Byte };

    auto calculatedUnit = SizeUnit::Byte;

    while (value > 1000)
    {
        value /= 1000;
        calculatedUnit = static_cast<SizeUnit>(static_cast<int>(calculatedUnit) + 1);
    }

    if (value > 100.0)
    {
        int roundedValue = static_cast<int>(value / 40) * 40;
        while (roundedValue < value)
            roundedValue += 40;
        return { static_cast<double>(roundedValue), calculatedUnit };
    }

    if (value > 10.0)
    {
        int roundedValue = static_cast<int>(value / 4) * 4;
        while (roundedValue < value)
            roundedValue += 4;
        return { static_cast<double>(roundedValue), calculatedUnit };
    }

    for (const auto &roundedValue : roundingTable)
    {
        if (value <= roundedValue)
            return { roundedValue, calculatedUnit };
    }

    return { 10.0, calculatedUnit };
}

QString formatLabel(const double argValue, const SizeUnit unit)
{
    // check is there need for digits after decimal separator
    const int precision = (argValue < 10) ? friendlyUnitPrecision(unit) : 0;
    return QLocale::system().toString(argValue, 'f', precision) + " " + unitString(unit, true);
}

void DownloaderSpeedWidget::replot()
{
    viewport()->update();
}

qint64 DownloaderSpeedWidget::maxYValue()
{
    qint64 maxYValue = 0;
    for(int id = 0; id < 2; id++)
        for (int i = speedRecorder_->dataCollection().size() - 1, j = 0; (i >= 0) && (j <= SpeedRecorder::DATA_MAXSIZE); --i, ++j){
            if (speedRecorder_->dataCollection()[i].y[id] > maxYValue)
                maxYValue = speedRecorder_->dataCollection()[i].y[id];
        }

    return maxYValue;
}

SpeedRecorder *DownloaderSpeedWidget::speedrecorder() const
{
    return speedRecorder_;
}

void DownloaderSpeedWidget::setSpeedRecorder(SpeedRecorder *newSpeedRecorder)
{
    disconnect(speedRecorder_, &SpeedRecorder::dataUpdated, this, &DownloaderSpeedWidget::replot);
    speedRecorder_ = newSpeedRecorder;
    connect(speedRecorder_, &SpeedRecorder::dataUpdated, this, &DownloaderSpeedWidget::replot);
    replot();
}

void DownloaderSpeedWidget::paintEvent(QPaintEvent *)
{
    if(!speedRecorder_) return;
    const auto fullRect = viewport()->rect();
    auto rect = viewport()->rect();
    rect.adjust(4, 4, 0, -4); // Add padding
    const auto niceScale = getRoundedYScale(maxYValue());
    // draw Y axis speed labels
    const QVector<QString> speedLabels = {
        formatLabel(niceScale.arg, niceScale.unit),
        formatLabel((0.75 * niceScale.arg), niceScale.unit),
        formatLabel((0.50 * niceScale.arg), niceScale.unit),
        formatLabel((0.25 * niceScale.arg), niceScale.unit),
        formatLabel(0.0, niceScale.unit),
    };

    QPainter painter(viewport());
    painter.setRenderHints(QPainter::Antialiasing);
    //
    const auto fontMetrics = painter.fontMetrics();
    rect.adjust(0, fontMetrics.height(), 0, 0); // Add top padding for top speed text
    //
    int yAxisWidth = 0;
    for (const auto &label : speedLabels)
        if (fontMetrics.horizontalAdvance(label) > yAxisWidth)
            yAxisWidth = fontMetrics.horizontalAdvance(label);

    int i = 0;
    for(const auto &label : speedLabels){
        QRectF labelRect(rect.topLeft() + QPointF(-yAxisWidth, (i++) * 0.25 * rect.height() - fontMetrics.height()),
                         QSizeF(2 * yAxisWidth, fontMetrics.height()));
        painter.drawText(labelRect, label, Qt::AlignRight | Qt::AlignTop);
    }

    // draw grid lines
    rect.adjust(yAxisWidth + 4, 0, 0, 0);
    QPen gridPen;
    gridPen.setStyle(Qt::DashLine);
    gridPen.setWidthF(1);
    gridPen.setColor(QColor(128, 128, 128, 128));
    // Set antialiasing for graphs
    painter.setPen(gridPen);
    painter.drawLine(fullRect.left(), rect.top(), rect.right(), rect.top());
    painter.drawLine(fullRect.left(), rect.top() + 0.25 * rect.height(), rect.right(), rect.top() + 0.25 * rect.height());
    painter.drawLine(fullRect.left(), rect.top() + 0.50 * rect.height(), rect.right(), rect.top() + 0.50 * rect.height());
    painter.drawLine(fullRect.left(), rect.top() + 0.75 * rect.height(), rect.right(), rect.top() + 0.75 * rect.height());
    painter.drawLine(fullRect.left(), rect.bottom(), rect.right(), rect.bottom());

    constexpr auto TIME_AXIS_DIVISIONS = 6;
    for(int i = 0; i < TIME_AXIS_DIVISIONS; ++i){
        const int x = rect.left() + (i * rect.width()) / TIME_AXIS_DIVISIONS;
        painter.drawLine(x, fullRect.top(), x, fullRect.bottom());
    }

    // draw graphs
    // Need, else graphs cross left gridline
    rect.adjust(3, 0, 0, 0);
    //
    const double yMultiplier = (niceScale.arg == 0.0) ? 0.0 : (static_cast<double>(rect.height()) / niceScale.sizeInBytes());
    const double xTickSize = static_cast<double>(rect.width()) / SpeedRecorder::DATA_MAXSIZE;

    for(int id = 0; id < pens_.size(); id++){
        QVector<QPoint> points;
        for(int i = speedRecorder_->dataCollection().size() - 1, j = 0; (i >= 0) && (j <= SpeedRecorder::DATA_MAXSIZE); --i, ++j){
            const int newX = rect.right() - j * xTickSize;
            const int newY = rect.bottom() - speedRecorder_->dataCollection()[i].y[id] * yMultiplier;
            points.push_back({ newX, newY });
        }
        painter.setPen(pens_.at(id).second);
        painter.drawPolyline(points.data(), points.size());
    }

    // draw legend
    double legendHeight = 0;
    int legendWidth = 0;

    for(const auto &[name, pen] : qAsConst(pens_)){
        if (fontMetrics.horizontalAdvance(name) > legendWidth)
            legendWidth = fontMetrics.horizontalAdvance(name);

        legendHeight += 1.5 * fontMetrics.height();
    }

    QPoint legendTopLeft(rect.left() + 4, fullRect.top() + 4);
    QRectF legendBackgroundRect(QPoint(legendTopLeft.x() - 4, legendTopLeft.y() - 4), QSizeF(legendWidth + 8, legendHeight + 8));
    auto legendBackgroundColor = QWidget::palette().color(QWidget::backgroundRole());
    legendBackgroundColor.setAlpha(128); // 50% transparent
    painter.fillRect(legendBackgroundRect, legendBackgroundColor);
    int j = 0;
    for(const auto &[name, pen] : qAsConst(pens_)){
        int nameSize = fontMetrics.horizontalAdvance(name);
        double indent = 1.5 * (j++) * fontMetrics.height();
        painter.setPen(pen);
        painter.drawLine(legendTopLeft + QPointF(0, indent + fontMetrics.height()),
                         legendTopLeft + QPointF(nameSize, indent + fontMetrics.height()));
        painter.drawText(QRectF(legendTopLeft + QPointF(0, indent), QSizeF(2 * nameSize, fontMetrics.height())), name,
                         QTextOption(Qt::AlignVCenter));
    }
}
