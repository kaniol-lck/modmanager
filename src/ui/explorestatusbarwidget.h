#ifndef EXPLORESTATUSBARWIDGET_H
#define EXPLORESTATUSBARWIDGET_H

#include <QWidget>

class QButtonGroup;
namespace Ui {
class ExploreStatusBarWidget;
}

class ExploreStatusBarWidget : public QWidget
{
    Q_OBJECT
public:
    enum ViewMode{ ListMode, IconMode, DetailMode };
    explicit ExploreStatusBarWidget(QWidget *parent = nullptr);
    ~ExploreStatusBarWidget();

public slots:
    void setProgressVisible(bool visible);
    void setText(const QString &text);
    void setModCount(int shownCount, int loadCount);

signals:
    void viewModeChanged(int mode);

private:
    Ui::ExploreStatusBarWidget *ui;
    QButtonGroup *group_;
};

#endif // EXPLORESTATUSBARWIDGET_H
