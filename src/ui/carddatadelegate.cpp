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
