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
    explicit WindowsTitleBar(QWidget *parent = nullptr, QMenuBar *menuBar = nullptr);
    ~WindowsTitleBar();

    //标题栏的总高与其中菜单栏的高度。原来这两个数分散在 .ui（36）和
    //updateMenuBar()（31+5px 外边距）里互相凑，改一处就会错位，这里统一到一处。
    static constexpr int kTitleBarHeight = 36;
    static constexpr int kMenuBarHeight = 31;
    //三个标题栏按钮的尺寸，取 Windows 惯例（Win10 约 45x30、Win11 46x32）。
    //原来在 .ui 里是 48 宽、竖向撑满整个标题栏高度，观感偏大、图标在按钮里显得空。
    static constexpr int kTitleButtonWidth = 46;
    static constexpr int kTitleButtonHeight = 32;

    void setIconVisible(bool bl);
    void setParentWidget(QWidget *newParentWidget);
#ifdef Q_OS_WIN
#if QT_VERSION_MAJOR == 6
    bool hitTest(QPoint pos, qintptr *result);
#else
    bool hitTest(QPoint pos, long *result);
#endif  // QT_VERSION_MAJOR
#endif //Q_OS_WIN

public slots:
    void updateMenuBar();
    void setMaximumed();
    void setNormal();

protected:
    void paintEvent(QPaintEvent *event) override;

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
