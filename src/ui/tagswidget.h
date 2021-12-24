#ifndef TAGSWIDGET_H
#define TAGSWIDGET_H

#include <QWidget>

#include "local/localmod.h"

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
    void setMod(T* tagableObject)
    {
        isLocalMod_ = std::is_base_of_v<T, LocalMod>;
        disconnect(conn_);
        tagableObject_ = tagableObject;
        updateUi();
        if(tagableObject_)
            conn_ = connect(tagableObject, &T::tagsChanged, this, &TagsWidget::updateUi);
    }

    void updateUi();
    bool iconOnly() const;
    void setIconOnly(bool newIconOnly);

protected:
    void wheelEvent(QWheelEvent *event) override;
private slots:
    void on_TagsWidget_customContextMenuRequested(const QPoint &pos);

private:
    QMetaObject::Connection conn_;
    Ui::TagsWidget *ui;
    QList<QWidget *> tagWidgets_;
    Tagable *tagableObject_ = nullptr;
    bool iconOnly_ = false;
    bool isLocalMod_ = false;
};

#endif // TAGSWIDGET_H
