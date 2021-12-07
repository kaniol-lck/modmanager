#ifndef CURSEFORGESTATUSBARWIDGET_H
#define CURSEFORGESTATUSBARWIDGET_H

#include <QWidget>

namespace Ui {
class CurseforgeStatusBarWidget;
}

class CurseforgeStatusBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CurseforgeStatusBarWidget(QWidget *parent = nullptr);
    ~CurseforgeStatusBarWidget();

public slots:
    void setProgressVisible(bool visible);
    void setText(const QString &text);
private:
    Ui::CurseforgeStatusBarWidget *ui;
};

#endif // CURSEFORGESTATUSBARWIDGET_H
