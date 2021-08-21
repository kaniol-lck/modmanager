#ifndef MODENTRYWIDGET_H
#define MODENTRYWIDGET_H

#include <QWidget>

#include "local/localmodinfo.h"

class QNetworkAccessManager;
class CurseforgeMod;

namespace Ui {
class LocalModItemWidget;
}

class LocalModItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LocalModItemWidget(QWidget *parent, QNetworkAccessManager *manager,  const LocalModInfo& modInfo);
    ~LocalModItemWidget();

    CurseforgeMod *getCurseforgeMod() const;

private:
    Ui::LocalModItemWidget *ui;
    LocalModInfo localModInfo;
    CurseforgeMod *curseforgeMod;

    QNetworkAccessManager *accessManager;

};

#endif // MODENTRYWIDGET_H
