#include "itemdelegate.h"

#include <QComboBox>

ItemDelegate::ItemDelegate()
{

}

QWidget *ItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return new QComboBox(parent);
}

void ItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{

}

void ItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{

}

void ItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

}
