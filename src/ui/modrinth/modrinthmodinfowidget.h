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
private:
    Ui::ModrinthModInfoWidget *ui;
    ModrinthMod *mod_ = nullptr;
    QList<QWidget*> tagWidgets_;
};

#endif // MODRINTHMODINFOWIDGET_H
