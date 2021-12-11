#ifndef LOCALMODINFOWIDGET_H
#define LOCALMODINFOWIDGET_H

#include "ui/dockwidgetcontent.h"

class LocalMod;

class LocalModPath;
namespace Ui {
class LocalModInfoWidget;
}

class LocalModInfoWidget : public DockWidgetContent
{
    Q_OBJECT

public:
    explicit LocalModInfoWidget(QWidget *parent = nullptr, LocalModPath *path = nullptr);
    ~LocalModInfoWidget();

    void setMods(QList<LocalMod *> mods);
signals:
    void modChanged();
private slots:
    void updateInfo();
    void updatePathInfo();
private:
    LocalModPath *path_ = nullptr;
    Ui::LocalModInfoWidget *ui;
    QList<LocalMod *> mods_;
    LocalMod * mod_;
};

#endif // LOCALMODINFOWIDGET_H
