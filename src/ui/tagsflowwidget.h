#ifndef TAGSFLOWWIDGET_H
#define TAGSFLOWWIDGET_H

#include <QWidget>

#include "local/localmod.h"

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
        isLocalMod_ = std::is_base_of_v<T, LocalMod>;
        if(tagableObject_) disconnect(tagableObject, &T::tagsChanged, this, &TagsFlowWidget::updateUi);
        tagableObject_ = tagableObject;
        updateUi();
        connect(tagableObject, &T::tagsChanged, this, &TagsFlowWidget::updateUi);
    }

    void updateUi();

private slots:
    void on_TagsFlowWidget_customContextMenuRequested(const QPoint &pos);

private:
    QList<QWidget*> tagWidgets_;
    Tagable *tagableObject_ = nullptr;
    bool isLocalMod_ = false;
};

#endif // TAGSFLOWWIDGET_H
