#ifndef TAGSFLOWWIDGET_H
#define TAGSFLOWWIDGET_H

#include <QWidget>

class LocalMod;

class TagsFlowWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TagsFlowWidget(QWidget *parent = nullptr, LocalMod *mod = nullptr);

    void setMod(LocalMod* mod);
    void updateUi();
private:
    QList<QWidget*> tagWidgets_;
    LocalMod *mod_ = nullptr;
};

#endif // TAGSFLOWWIDGET_H
