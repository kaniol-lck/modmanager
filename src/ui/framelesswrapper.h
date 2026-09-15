#ifndef FRAMELESSWRAPPER_H
#define FRAMELESSWRAPPER_H

#include <QMainWindow>

class WindowsTitleBar;

class FramelessWrapper : public QMainWindow
{
    Q_OBJECT

public:
    FramelessWrapper(QMainWindow *window);
    FramelessWrapper(QDialog *dialog);
    FramelessWrapper(QWidget *widget, QMenuBar *menuBar = nullptr);

    //外观开关（模糊/无边框）改动后刷新所有存活实例。
    //标题栏与模糊状态都是按需读 Config 的，所以只需要通知它们重绘/重设一次。
    static void updateAllBlur();

public slots:
    void updateBlur();

protected:
#ifdef Q_OS_WIN
#if QT_VERSION_MAJOR == 6
    virtual bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;
#else
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
#endif // QT_VERSION_MAJOR
#endif // Q_OS_WIN

private:
    WindowsTitleBar *titleBar_;
};

#endif // FRAMELESSWRAPPER_H
