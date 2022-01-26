#ifndef BROWSER_H
#define BROWSER_H

#include <QMainWindow>

class Browser : public QMainWindow
{
    Q_OBJECT
public:
    explicit Browser(QWidget *parent = nullptr);
    virtual ~Browser();

    void show();

    virtual void load();

    virtual void updateUi();

    virtual QWidget *infoWidget() const;
    virtual QWidget *fileListWidget() const;
    virtual void onModMenuAboutToShow();
    virtual void onModMenuAboutToHide();
    virtual QIcon icon() const = 0;
    virtual QString name() const = 0;
};

#endif // BROWSER_H
