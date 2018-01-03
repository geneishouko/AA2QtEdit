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

#include "carddataview.h"

#include <QApplication>
#include <QPainter>

using namespace ClassEdit;

CardDataView::CardDataView(QWidget *parent) :
    QTreeView (parent)
{
    setEditTriggers(CurrentChanged);
    setTabKeyNavigation(false);
    connect(this, &QAbstractItemView::clicked, this, &CardDataView::toggleCollapse);
}

void CardDataView::toggleCollapse(const QModelIndex &index)
{
    QModelIndex root = index.sibling(index.row(), 0);
    if (model()->rowCount(root))
        setExpanded(root, !isExpanded(root));
}

void CardDataView::drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const
{
    Q_UNUSED(index)
    painter->fillRect(rect, QBrush(qApp->palette().midlight()));
    QTreeView::drawBranches(painter, rect, index);
}
