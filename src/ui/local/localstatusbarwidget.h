#ifndef LOCALSTATUSBARWIDGET_H
#define LOCALSTATUSBARWIDGET_H

#include <QWidget>

class QButtonGroup;

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

    QProgressBar *progressBar() const;

    void setText(const QString &text);
signals:
    void viewModeChanged(int mode);
private:
    Ui::LocalModStatusBar *ui;
    QButtonGroup *group_;
};

#endif // LOCALSTATUSBARWIDGET_H
