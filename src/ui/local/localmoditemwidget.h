#ifndef MODENTRYWIDGET_H
#define MODENTRYWIDGET_H

#include <QWidget>

#include "local/localmod.h"

namespace Ui {
class LocalModItemWidget;
}

class QMenu;

class LocalModItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LocalModItemWidget(QWidget *parent, LocalMod *localMod);
    ~LocalModItemWidget();

    LocalMod *mod() const;

    void setMinimal(bool minimal);

public slots:
    void updateInfo();
    void updateUi();

private slots:
    void onIconChanged();

    void on_updateButton_clicked();

    void updateReady();

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
protected:
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *event) override;
#else
    void enterEvent(QEvent * event) override;
#endif
    void leaveEvent(QEvent * event) override;
    void paintEvent(QPaintEvent *event) override;
private:
    Ui::LocalModItemWidget *ui;
    LocalMod *mod_;
    // 菜单只建一次、每次刷新时清空内容。
    // 原来 updateInfo()/updateReady() 每次都 new 一个 QMenu 并 setMenu()，旧菜单从不 delete，
    // 而 modInfoChanged 触发得相当频繁 —— 子对象会持续累积。
    QMenu *rollbackMenu_ = nullptr;
    QMenu *updateMenu_ = nullptr;
    QMenu *ignoreUpdateMenu_ = nullptr;
};

#endif // MODENTRYWIDGET_H
