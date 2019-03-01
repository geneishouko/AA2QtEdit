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

#include "cardlistview.h"

#include "cardlistdelegate.h"

#include <QMimeData>
#include <QMouseEvent>

using namespace ClassEdit;

CardListView::CardListView(QWidget *parent) :
    QListView(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setMouseTracking(true);
    m_delegate = new CardListDelegate(this);
    setItemDelegate(m_delegate);
    setAcceptDrops(true);
}

void CardListView::mouseMoveEvent(QMouseEvent *event)
{
    QModelIndex itemIndex = indexAt(event->pos());
    if (!itemIndex.isValid())
        return;
    if (m_delegate->needsUpdate(event->pos()))
        update(itemIndex);
}

void CardListView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->accept();
}

void CardListView::dragMoveEvent(QDragMoveEvent *)
{
}

void CardListView::dropEvent(QDropEvent *event)
{
    event->accept();
    QFileInfoList files;
    foreach(const QUrl &url, event->mimeData()->urls()) {
        files << url.toLocalFile();
    }
    emit droppedFiles(files);
}
