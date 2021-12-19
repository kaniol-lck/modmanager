#include "datetimetext.h"
#include "ui_datetimetext.h"

#include "util/funcutil.h"

DateTimeText::DateTimeText(QWidget *parent, const QDateTime &dateTime) :
    QWidget(parent),
    ui(new Ui::DateTimeText)
{
    ui->setupUi(this);
    setProperty("class", "DateTimeText");
    ui->label->setProperty("class", "TimeHint");
    ui->fileDateText->setProperty("class", "Time");
    setDateTime(dateTime);
}

DateTimeText::~DateTimeText()
{
    delete ui;
}

void DateTimeText::setText(const QString &str)
{
    ui->label->setText(str);
}

void DateTimeText::setDateTime(const QDateTime &dateTime)
{
    ui->fileDateText->setText(tr("%1 ago").arg(timesTo(dateTime)));
    ui->fileDateText->setToolTip(dateTime.toString());
}
