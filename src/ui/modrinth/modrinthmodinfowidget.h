#ifndef MODRINTHMODINFOWIDGET_H
#define MODRINTHMODINFOWIDGET_H

#include <QWidget>

class ModrinthMod;
namespace Ui {
class ModrinthModInfoWidget;
}

class ModrinthModInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModrinthModInfoWidget(QWidget *parent = nullptr);
    ~ModrinthModInfoWidget();

    void setMod(ModrinthMod *mod);
signals:
    void modChanged();
private slots:
    void updateBasicInfo();
    void updateFullInfo();
    void updateIcon();
    void on_modSummary_customContextMenuRequested(const QPoint &pos);

private:
    Ui::ModrinthModInfoWidget *ui;
    ModrinthMod *mod_ = nullptr;
    QList<QWidget*> tagWidgets_;
    bool transltedSummary_ = false;
};

#endif // MODRINTHMODINFOWIDGET_H
