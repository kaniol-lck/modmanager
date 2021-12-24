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
        disconnect(conn_);
        tagableObject_ = tagableObject;
        updateUi();
        if(tagableObject_)
            conn_ = connect(tagableObject, &T::tagsChanged, this, &TagsFlowWidget::updateUi);
    }

    void updateUi();

private slots:
    void on_TagsFlowWidget_customContextMenuRequested(const QPoint &pos);

private:
    QMetaObject::Connection conn_;
    QList<QWidget*> tagWidgets_;
    Tagable *tagableObject_ = nullptr;
    bool isLocalMod_ = false;
};

#endif // TAGSFLOWWIDGET_H
