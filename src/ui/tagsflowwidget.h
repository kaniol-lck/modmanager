#ifndef TAGSFLOWWIDGET_H
#define TAGSFLOWWIDGET_H

#include <QWidget>

class Tagable;
class TagsFlowWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TagsFlowWidget(QWidget *parent = nullptr);

    template<typename T>
    void setMod(T* mod)
    {
        if(mod_) disconnect(mod, &T::tagsChanged, this, &TagsFlowWidget::updateUi);
        mod_ = mod;
        updateUi();
        connect(mod, &T::tagsChanged, this, &TagsFlowWidget::updateUi);
    }

    void updateUi();
private:
    QList<QWidget*> tagWidgets_;
    Tagable *mod_ = nullptr;
};

#endif // TAGSFLOWWIDGET_H
