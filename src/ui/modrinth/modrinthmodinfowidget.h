#ifndef MODRINTHMODINFOWIDGET_H
#define MODRINTHMODINFOWIDGET_H

#include <QWidget>

class ModrinthMod;
class ModrinthModBrowser;
namespace Ui {
class ModrinthModInfoWidget;
}

class ModrinthModInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModrinthModInfoWidget(ModrinthModBrowser *parent = nullptr);
    ~ModrinthModInfoWidget();

    void setMod(ModrinthMod *mod);
signals:
    void modChanged();
private slots:
    void updateBasicInfo();
    void updateFullInfo();
    void updateIcon();
    void on_modSummary_customContextMenuRequested(const QPoint &pos);

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    Ui::ModrinthModInfoWidget *ui;
    ModrinthModBrowser *browser_;
    ModrinthMod *mod_ = nullptr;
    bool transltedSummary_ = false;
};

#endif // MODRINTHMODINFOWIDGET_H
