#ifndef CURSEFORGEMODINFOWIDGET_H
#define CURSEFORGEMODINFOWIDGET_H

#include <QWidget>

class CurseforgeMod;
class CurseforgeModBrowser;
namespace Ui {
class CurseforgeModInfoWidget;
}

class CurseforgeModInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CurseforgeModInfoWidget(CurseforgeModBrowser *parent = nullptr);
    ~CurseforgeModInfoWidget();

    void setMod(CurseforgeMod *mod);

signals:
    void modChanged();

private slots:
    void updateBasicInfo();
    void updateThumbnail();
    void updateDescription();
    void on_modSummary_customContextMenuRequested(const QPoint &pos);
    void on_actionOpen_Curseforge_Mod_Dialog_triggered();
    void on_actionOpen_Website_Link_triggered();
    void on_actionCopy_Website_Link_triggered();

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    Ui::CurseforgeModInfoWidget *ui;
    CurseforgeModBrowser *browser_;
    CurseforgeMod *mod_ = nullptr;
    bool transltedSummary_ = false;
};

#endif // CURSEFORGEMODINFOWIDGET_H
