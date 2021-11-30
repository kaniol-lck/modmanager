#ifndef BROWSER_H
#define BROWSER_H

#include <QWidget>

class Browser : public QWidget
{
    Q_OBJECT
public:
    explicit Browser(QWidget *parent = nullptr);
    virtual ~Browser() = 0;

    virtual QWidget *infoWidget() const;
    virtual QWidget *fileListWidget() const;
    virtual QList<QAction *> modActions() const;
    virtual void onModMenuAboutToShow();
    virtual void onModMenuAboutToHide();
};

#endif // BROWSER_H
