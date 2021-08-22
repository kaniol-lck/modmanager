#ifndef MODENTRYWIDGET_H
#define MODENTRYWIDGET_H

#include <QWidget>

#include "local/localmod.h"

class QNetworkAccessManager;
class CurseforgeMod;
class LocalMod;

namespace Ui {
class LocalModItemWidget;
}

class LocalModItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LocalModItemWidget(QWidget *parent, LocalMod *localMod);
    ~LocalModItemWidget();

public slots:
    void needUpdate();

private slots:
    void on_updateButton_clicked();

private:
    Ui::LocalModItemWidget *ui;
    LocalMod *localMod;

};

#endif // MODENTRYWIDGET_H
