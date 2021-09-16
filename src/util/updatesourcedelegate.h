#ifndef COMBODELEGATE_H
#define COMBODELEGATE_H

#include <QItemDelegate>

#include "local/localmod.h"

class UpdateSourceDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    UpdateSourceDelegate(QList<LocalMod::ModWebsiteType> typeList, QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,	const QStyleOptionViewItem &option, const QModelIndex &index) const;
    int nOpIndex = 0;

    static QList<QPair<QString, QString>> sourceItems;
signals:
    void updateSourceChanged(int row, LocalMod::ModWebsiteType type) const;

private:
    QList<LocalMod::ModWebsiteType> typeList_;
};

#endif // COMBODELEGATE_H
