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

    void enterEvent(QEvent * event);
    void leaveEvent(QEvent * event);

    LocalMod *mod() const;

public slots:
    void updateInfo();

private slots:
    void on_updateButton_clicked();

    void updateReady(LocalMod::ModWebsiteType type);

    void startCheckCurseforge();
    void curseforgeReady(bool bl);
    void startCheckCurseforgeUpdate();

    void startCheckModrinth();
    void modrinthReady(bool bl);
    void startCheckModrinthUpdate();

    void startUpdate();
    void updateProgress(qint64 bytesReceived, qint64 bytesTotal);
    void finishUpdate(bool success);

    void on_curseforgeButton_clicked();

    void on_modrinthButton_clicked();

    void on_warningButton_clicked();

    void on_disableButton_toggled(bool checked);

    void on_featuredButton_toggled(bool checked);

    void on_LocalModItemWidget_customContextMenuRequested(const QPoint &pos);

private:
    Ui::LocalModItemWidget *ui;
    QList<QWidget *> tagWidgets_;
    LocalMod *mod_;
};

#endif // MODENTRYWIDGET_H
