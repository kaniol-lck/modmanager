#ifndef WINDOWSTITLEBAR_H
#define WINDOWSTITLEBAR_H

#include <QWidget>

class QMenuBar;
class QToolButton;
namespace Ui {
class WindowsTitleBar;
}

class WindowsTitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit WindowsTitleBar(QWidget *parent = nullptr, const QString &title = QString(), QMenuBar *menuBar = nullptr);
    ~WindowsTitleBar();

    void setIconVisible(bool bl);
    void setParentWidget(QWidget *newParentWidget);

#ifdef Q_OS_WIN
    bool hitTest(QPoint pos, long *result);
#endif //Q_OS_WIN

public slots:
    void updateMenuBar();

    void setMaximumed();
    void setNormal();

protected:
#ifdef Q_OS_WIN
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
#endif //Q_OS_WIN
private slots:
    void on_closeButton_clicked();
    void on_maxButton_clicked();
    void on_minButton_clicked();

private:
    Ui::WindowsTitleBar *ui;
    QWidget *parentWidget_;
    QMenuBar *menuBar_;
    QPoint clickPos_;
};

#endif // WINDOWSTITLEBAR_H
