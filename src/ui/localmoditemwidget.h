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
    void updateReady(bool need);

private slots:
    void on_updateButton_clicked();

    void startCheckCurseforge();
    void curseforgeReady();
    void startCheckUpdate();
    void startUpdate();
    void updateProgress(qint64 bytesReceived, qint64 bytesTotal);
    void finishUpdate();

private:
    Ui::LocalModItemWidget *ui;
    LocalMod *localMod;

};

#endif // MODENTRYWIDGET_H
