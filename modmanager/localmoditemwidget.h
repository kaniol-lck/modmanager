#ifndef MODENTRYWIDGET_H
#define MODENTRYWIDGET_H

#include <QWidget>

class LocalModInfo;

namespace Ui {
class LocalModItemWidget;
}

class LocalModItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LocalModItemWidget(QWidget *parent, const LocalModInfo& modInfo);
    ~LocalModItemWidget();

private:
    Ui::LocalModItemWidget *ui;
};

#endif // MODENTRYWIDGET_H
