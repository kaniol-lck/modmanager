#ifndef FRAMELESSWRAPPER_H
#define FRAMELESSWRAPPER_H

#include <QMenuBar>
#include <QWidget>

class QMainWindow;
class WindowsTitleBar;
class FramelessWrapper : QWidget
{
    Q_OBJECT

public:
    FramelessWrapper(QWidget *parent, QWidget *widget, QMenuBar *menuBar);
    static QWidget *makeFrameless(QMainWindow *window);

protected:
#ifdef Q_OS_WIN
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
#endif //Q_OS_WIN

private:
    WindowsTitleBar *titleBar_;
};

#endif // FRAMELESSWRAPPER_H
