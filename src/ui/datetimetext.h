#ifndef DATETIMETEXT_H
#define DATETIMETEXT_H

#include <QWidget>

#include <QDateTime>

namespace Ui {
class DateTimeText;
}

class DateTimeText : public QWidget
{
    Q_OBJECT

public:
    explicit DateTimeText(QWidget *parent = nullptr, const QDateTime &dateTime = QDateTime());
    ~DateTimeText();

    void setText(const QString &str);
    void setDateTime(const QDateTime &dateTime);

private:
    Ui::DateTimeText *ui;
};

#endif // DATETIMETEXT_H
