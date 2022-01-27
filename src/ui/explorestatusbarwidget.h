#ifndef EXPLORESTATUSBARWIDGET_H
#define EXPLORESTATUSBARWIDGET_H

#include <QWidget>

namespace Ui {
class ExploreStatusBarWidget;
}

class ExploreStatusBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExploreStatusBarWidget(QWidget *parent = nullptr);
    ~ExploreStatusBarWidget();

public slots:
    void setProgressVisible(bool visible);
    void setText(const QString &text);
    void setModCount(int loadCount);

private:
    Ui::ExploreStatusBarWidget *ui;
};

#endif // EXPLORESTATUSBARWIDGET_H
