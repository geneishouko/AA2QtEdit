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

#include "carddatadelegate.h"

#include "coloritemeditor.h"
#include "../datareader.h"
#include "../dictionary.h"

#include <QComboBox>
#include <QPainter>

using namespace ClassEdit;

CardDataDelegate::CardDataDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

QWidget *CardDataDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *editor = nullptr;
    if (index.data(Dictionary::DataTypeRole) == DataType::Enum) {
        QComboBox *cb = new QComboBox(parent);
        cb->setEditable(true);
        const DataEnumerable *enumerable = DataReader::getDataEnumerable(index.data(Dictionary::MetaKeyRole).toString());
        int value = index.data(Qt::EditRole).toInt();
        int i = 0;
        bool enumFound = false;
        for (QMap<qint32, QString>::ConstIterator it = enumerable->valueMap().begin(); it != enumerable->valueMap().end(); it++) {
            cb->insertItem(i, it.value(), it.key());
            if (value == it.key()) {
                enumFound = true;
                cb->setCurrentIndex(i);
            }
            i++;
        }
        if (!enumFound) {
            cb->setCurrentIndex(-1);
            cb->setCurrentText(QString::number(value, 10));
        }
        editor = cb;
    }
    else {
        editor = QStyledItemDelegate::createEditor(parent, option, index);
    }
    return editor;
}

void CardDataDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *cb = qobject_cast<QComboBox*>(editor);
    if (!cb)
        QStyledItemDelegate::setEditorData(editor, index);
}

void CardDataDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *cb = qobject_cast<QComboBox*>(editor);
    if (cb) {
        QVariant data = cb->currentData();
        if (!data.isValid()) {
            bool ok;
            int number = cb->currentText().toInt(&ok);
            if (ok)
                data = number;
        }
        if (data.isValid())
            model->setData(index, data);
    }
    else
        QStyledItemDelegate::setModelData(editor, model, index);
}
