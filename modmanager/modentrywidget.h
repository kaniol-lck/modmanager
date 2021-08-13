#ifndef MODENTRYWIDGET_H
#define MODENTRYWIDGET_H

#include <QWidget>

class ModInfo;

namespace Ui {
class ModEntryWidget;
}

class ModEntryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModEntryWidget(QWidget *parent, const ModInfo& modInfo);
    ~ModEntryWidget();

private:
    Ui::ModEntryWidget *ui;
};

#endif // MODENTRYWIDGET_H
