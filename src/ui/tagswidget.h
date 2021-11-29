#ifndef TAGSWIDGET_H
#define TAGSWIDGET_H

#include <QWidget>

class LocalMod;
namespace Ui {
class TagsWidget;
}
class TagsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TagsWidget(QWidget *parent = nullptr, LocalMod* mod = nullptr);

    void setMod(LocalMod* mod);
    void updateUi();
protected:
    void wheelEvent(QWheelEvent *event) override;
private:
    Ui::TagsWidget *ui;
    QList<QWidget *> tagWidgets_;
    LocalMod *mod_ = nullptr;
};

#endif // TAGSWIDGET_H
