#ifndef FRAMELESSWRAPPER_H
#define FRAMELESSWRAPPER_H

#include <QMenuBar>
#include <QMainWindow>

class QMainWindow;
class WindowsTitleBar;

class FramelessWrapper : public QMainWindow
{
    Q_OBJECT

public:
    FramelessWrapper(QWidget *parent, QWidget *widget, QMenuBar *menuBar = nullptr);
    FramelessWrapper(QWidget *parent, QWidget *widget, WindowsTitleBar *titleBar);

private slots:
    void updateBlur();

protected:
#ifdef Q_OS_WIN
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);
#endif //Q_OS_WIN

private:
    WindowsTitleBar *titleBar_;
    bool isMoving_ = false;
};

#endif // FRAMELESSWRAPPER_H
