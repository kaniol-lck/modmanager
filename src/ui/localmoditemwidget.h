#ifndef MODENTRYWIDGET_H
#define MODENTRYWIDGET_H

#include <QWidget>

#include "local/localmod.h"

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
    void updateInfo();

private slots:
    void on_updateButton_clicked();

    void curseforgeUpdateReady(bool need);
    void startCheckCurseforge();
    void curseforgeReady(bool bl);
    void startCheckCurseforgeUpdate();

    void modrinthUpdateReady(bool need);
    void startCheckModrinth();
    void modrinthReady(bool bl);
    void startCheckModrinthUpdate();

    void startUpdate();
    void updateProgress(qint64 bytesReceived, qint64 bytesTotal);
    void finishUpdate();

    void on_curseforgeButton_clicked();

    void on_modrinthButton_clicked();

private:
    Ui::LocalModItemWidget *ui;
    LocalMod *localMod;

};

#endif // MODENTRYWIDGET_H
