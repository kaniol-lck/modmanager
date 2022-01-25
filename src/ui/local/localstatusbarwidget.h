#ifndef LOCALSTATUSBARWIDGET_H
#define LOCALSTATUSBARWIDGET_H

#include <QWidget>

class QButtonGroup;

class QLabel;
class QProgressBar;

namespace Ui {
class LocalModStatusBar;
}

class LocalStatusBarWidget : public QWidget
{
    Q_OBJECT
public:
    enum ViewMode{ ListMode, IconMode, DetailMode };

    explicit LocalStatusBarWidget(QWidget *parent = nullptr);
    ~LocalStatusBarWidget();

    QLabel *label() const;
    QProgressBar *progressBar() const;

    void setModCount(int hiddenCount, int totalCount);

signals:
    void viewModeChanged(int mode);

private:
    Ui::LocalModStatusBar *ui;
    QButtonGroup *group_;
};

#endif // LOCALSTATUSBARWIDGET_H
