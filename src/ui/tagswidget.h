#ifndef TAGSWIDGET_H
#define TAGSWIDGET_H

#include <QWidget>

class Tagable;
namespace Ui {
class TagsWidget;
}
class TagsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TagsWidget(QWidget *parent = nullptr);
    ~TagsWidget();

    template<typename T>
    void setMod(T* mod)
    {
        if(mod_) disconnect(mod, &T::tagsChanged, this, &TagsWidget::updateUi);
        mod_ = mod;
        updateUi();
        connect(mod, &T::tagsChanged, this, &TagsWidget::updateUi);
    }

    void updateUi();
    bool iconOnly() const;
    void setIconOnly(bool newIconOnly);

protected:
    void wheelEvent(QWheelEvent *event) override;
private:
    Ui::TagsWidget *ui;
    QList<QWidget *> tagWidgets_;
    Tagable *mod_ = nullptr;
    bool iconOnly_ = false;
};

#endif // TAGSWIDGET_H
