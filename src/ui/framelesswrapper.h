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

private slots:
    void updateBlur();

protected:
#ifdef Q_OS_WIN
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);
#endif //Q_OS_WIN

private:
    WindowsTitleBar *titleBar_;
};

#endif // FRAMELESSWRAPPER_H
