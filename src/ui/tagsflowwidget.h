#ifndef TAGSFLOWWIDGET_H
#define TAGSFLOWWIDGET_H

#include <QWidget>

class Tagable;
class TagsFlowWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TagsFlowWidget(QWidget *parent = nullptr);

    void setTagable(const Tagable &tagable);

    template<typename T>
    void setTagableObject(T* tagableObject)
    {
        if(tagableObject_) disconnect(tagableObject, &T::tagsChanged, this, &TagsFlowWidget::updateUi);
        tagableObject_ = tagableObject;
        updateUi();
        connect(tagableObject, &T::tagsChanged, this, &TagsFlowWidget::updateUi);
    }

    void updateUi();
private:
    QList<QWidget*> tagWidgets_;
    Tagable *tagableObject_ = nullptr;
};

#endif // TAGSFLOWWIDGET_H
