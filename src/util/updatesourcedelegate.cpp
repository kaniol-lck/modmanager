#include "updatesourcedelegate.h"

#include <QComboBox>
#include <QDebug>

QList<QPair<QString, QString>> UpdateSourceDelegate::sourceItems{
    QPair("", ""), //placeholder for None
    QPair("Curseforge", ":/image/curseforge.svg"),
    QPair("Modrinth", ":/image/modrinth.svg")
};

UpdateSourceDelegate::UpdateSourceDelegate(QList<LocalMod::ModWebsiteType> typeList, QObject *parent) :
    QItemDelegate(parent),
    typeList_(typeList)
{}

QWidget *UpdateSourceDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
    QComboBox *editor = new QComboBox(parent);
    for(const auto &type : typeList_){
        auto item = sourceItems.at(type);
        editor->addItem(QIcon(item.second), item.first);
    }
    editor->setEditable(false);
    return editor;
}

void UpdateSourceDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::DisplayRole).toString();
    QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
    int icurIndex = comboBox->findText(value);
    comboBox->setCurrentIndex(icurIndex);
}

void UpdateSourceDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
    if(comboBox->currentIndex() == -1) return;
    auto item = sourceItems.at(typeList_.at(comboBox->currentIndex()));
    model->setData(index, item.first, Qt::DisplayRole);
    model->setData(index, QIcon(item.second), Qt::DecorationRole);
    emit updateSourceChanged(index.row(), typeList_.at(comboBox->currentIndex()));
}

void UpdateSourceDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
