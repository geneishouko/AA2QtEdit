/*
    This file is part of AA2QtEdit.

    AA2QtEdit is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AA2QtEdit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with AA2QtEdit.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "cardlistdelegate.h"

#include "../cardfile.h"
#include "../cardlistmodel.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QSortFilterProxyModel>

using namespace ClassEdit;

static const QSize cardSize(74, 90);
static const QPoint cardMargin(4, 4);
static const int itemVerticalMargin = 4;
static QSize buttonSize;
static QPoint mousePos;

CardListDelegate::CardListDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

bool CardListDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        bool hasPendingChanges = index.data(CardHasModifications).toBool();
        QMouseEvent *mouseEvent = reinterpret_cast<QMouseEvent*>(event);
        if (hasPendingChanges && saveButtonRect(option.rect).contains(mouseEvent->pos())) {
            static_cast<CardListModel*>(static_cast<QSortFilterProxyModel*>(model)->sourceModel())->save(index);
            return true;
        }
    }
    return false;
}

void CardListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyle *style = QApplication::style();

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    const QRect &rect = option.rect;

    QString saveButtonText("Save");
    QStyleOptionButton saveButtonOption;
    const QFontMetrics &buttonMetrics = saveButtonOption.fontMetrics;
    QSize buttonTextSize = buttonMetrics.boundingRect(saveButtonText).size();
    buttonSize = style->sizeFromContents(QStyle::CT_PushButton, &saveButtonOption, buttonTextSize, nullptr);
    const QRect &buttonRect = saveButtonRect(rect);
    if (buttonRect.contains(mousePos))
        saveButtonOption.state |= QStyle::State_HasFocus;
    saveButtonOption.text = saveButtonText;
    saveButtonOption.rect = buttonRect;

    QRect cardRect(rect.topLeft() + cardMargin, cardSize);
    QPoint nameBaseLine(90, rect.top() + 4 + option.fontMetrics.ascent());

    painter->save();
    QBrush currentBrush = painter->brush();
    QPen currentPen = painter->pen();
    painter->setBrush(Qt::lightGray);
    painter->setPen(Qt::NoPen);
    painter->drawRect(cardRect.adjusted(-2, 2, -2, 2));
    painter->setBrush(currentBrush);
    painter->setPen(currentPen);
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, nullptr);

    CardFile *card = index.data(CardFileRole).value<CardFile*>();
    bool hasPendingChanges = index.data(CardHasModifications).toBool();
    painter->drawPixmap(cardRect, card->thumbnailPixmap());
    if (card->seat() < 0)
        painter->drawText(rect.adjusted(cardRect.right() + 4, itemVerticalMargin, 0, 0), Qt::AlignLeft | Qt::TextSingleLine, card->fullName());
    else
        painter->drawText(rect.adjusted(cardRect.right() + 4, itemVerticalMargin, 0, 0), Qt::AlignLeft | Qt::TextSingleLine,
                          QString("[%1] %2").arg(card->seat()).arg(card->fullName()));
    painter->drawText(rect.adjusted(cardRect.right() + 4, itemVerticalMargin * 4, 0, 0), Qt::AlignLeft | Qt::TextSingleLine, card->fileName());
    if (hasPendingChanges)
        style->drawControl(QStyle::CE_PushButton, &saveButtonOption, painter, nullptr);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(cardRect);
    painter->restore();
}

QSize CardListDelegate::sizeHint(const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
    return QSize(260, itemVerticalMargin * 2 + cardSize.height());
}

bool CardListDelegate::needsUpdate(const QPoint &pos)
{
    mousePos = pos;
    return true;
}

QRect CardListDelegate::saveButtonRect(const QRect &itemRect) const
{
    return QRect(itemRect.right() - buttonSize.width() - 4, itemRect.top() + 4, buttonSize.width(), buttonSize.height());
}
