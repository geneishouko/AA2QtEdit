#include "carddatadelegate.h"

#include "coloritemeditor.h"

#include <QPainter>

using namespace ClassEdit;

CardDataDelegate::CardDataDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

QWidget *CardDataDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *editor = QStyledItemDelegate::createEditor(parent, option, index);
    ColorItemEditor *colorEditor = qobject_cast<ColorItemEditor*>(editor);
    if (colorEditor) {
        colorEditor->setFocus();
    }
    return editor;
}

void CardDataDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant data = index.data(Qt::DisplayRole);
    if (data.type() == QVariant::Color) {
        painter->save();
        painter->setBrush(QBrush(data.value<QColor>()));
        painter->drawRect(option.rect);
        painter->restore();
    }
    else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}
