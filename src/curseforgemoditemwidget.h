#ifndef CURSEFORGEMODITEMWIDGET_H
#define CURSEFORGEMODITEMWIDGET_H

#include <QWidget>
#include <QTimer>

class CurseforgeMod;

namespace Ui {
class CurseforgeModItemWidget;
}

class CurseforgeModItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CurseforgeModItemWidget(QWidget *parent, CurseforgeMod *mod);
    ~CurseforgeModItemWidget();

private slots:
    void updateThumbnail();
    void updateDownlaodSpeed();

    void on_downloadButton_clicked();

private:
    Ui::CurseforgeModItemWidget *ui;
    CurseforgeMod *curseforgeMod;
    QTimer speedTimer;
    qint64 lastDownloadBytes = 0;
};

#endif // CURSEFORGEMODITEMWIDGET_H
