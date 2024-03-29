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

#include "carddatamodel.h"

#include "cardfile.h"

#include <QApplication>
#include <QBrush>
#include <QPalette>

#include <QtDebug>

using namespace ClassEdit;

enum ModelHeaders {
    OffsetHeader,
    TypeHeader,
    KeyHeader,
    ValueHeader,
    HeaderCount
};

CardDataModel::CardDataModel(Dictionary *dictionary) :
    QAbstractItemModel(dictionary),
    m_dictionary(dictionary)
{
}

int CardDataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return HeaderCount;
}

Dictionary *CardDataModel::getDictionaryForIndex(const QModelIndex &index) const
{
    Dictionary *dictionary = static_cast<Dictionary*>(index.internalPointer());
    if (!dictionary)
        dictionary = m_dictionary;
    return dictionary;
}

Qt::ItemFlags CardDataModel::flags(const QModelIndex &index) const
{
    if (index.column() != ValueHeader)
        return Qt::ItemIsEnabled;
    QVariant value = getEntryForIndex(index);
    Dictionary *dictionary = value.value<Dictionary*>();
    Qt::ItemFlags f = Qt::NoItemFlags;
    QMetaType::Type valueType = static_cast<QMetaType::Type>(value.typeId());
    if (!dictionary && valueType != QMetaType::UnknownType && valueType != QMetaType::QByteArray)
        f |= Qt::ItemNeverHasChildren | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (valueType == QMetaType::Bool)
        f |= Qt::ItemIsUserCheckable;
    else
        f |= Qt::ItemIsEditable;
    return f;
}

int CardDataModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return static_cast<int>(m_dictionary->displayKeyList().size());
    Dictionary *dictionary = getEntryForIndex(parent).value<Dictionary*>();
    if (dictionary)
        return static_cast<int>(dictionary->displayKeyList().count());
    return 0;
}

QVariant CardDataModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        Dictionary *dictionary = getDictionaryForIndex(index);
        QVariant value = dictionary->at(index.row());
        DataType dataType = dictionary->dataType(index.row());
        switch (index.column()) {
            case OffsetHeader:
            return QString::asprintf("0x%.4X", dictionary->offset(index.row()));

            case TypeHeader:
            return dictionary->typeName(index.row());

            case KeyHeader:
            return dictionary->keyAt(index.row());

            case ValueHeader:
            switch (dataType) {
            case DataType::Array:
            case DataType::Struct: {
                Dictionary *dictionary = value.value<Dictionary*>();
                return tr("(%1 items)", nullptr, static_cast<int>(dictionary->count())).arg(dictionary->count());
            }
            case DataType::Enum:
                return dictionary->enumerable(index.row());
            case DataType::Byte:
            case DataType::Int16:
            case DataType::Int32:
            case DataType::Color:
            case DataType::String:
            case DataType::EncodedString:
                return value;
            case DataType::Bool:
            case DataType::Dummy:
            case DataType::Invalid:
                return QVariant();
            }
        }
    }
    else if (role == Qt::CheckStateRole) {
        if(index.column() == ValueHeader && getEntryForIndex(index).typeId() == QMetaType::Bool) {
            return getEntryForIndex(index).toBool() ? Qt::Checked : Qt::Unchecked;
        }
    }
    else if (role == Qt::EditRole) {
        return getEntryForIndex(index);
    }
    else if (role == Qt::BackgroundRole && index.column() != ValueHeader) {
        return QBrush(qApp->palette().midlight());
    }
    else if (role == Qt::DecorationRole && index.column() == ValueHeader) {
        QVariant value = getEntryForIndex(index);
        if (value.typeId() == QMetaType::QColor)
            return value;
    }
    else if (role == Dictionary::DataTypeRole) {
        return getDictionaryForIndex(index)->dataType(index.row());
    }
    else if (role == Dictionary::MetaKeyRole) {
        return getDictionaryForIndex(index)->metaKey(index.row());
    }
    return QVariant();
}

QVariant CardDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        QString header;
        switch (section) {
        case OffsetHeader:
            header = tr("Offset");
            break;
        case TypeHeader:
            header = tr("Type");
            break;
        case KeyHeader:
            header = tr("Key");
            break;
        case ValueHeader:
            header = tr("Value");
            break;
        case HeaderCount:
            Q_ASSERT_X(0, "CardDataModel Header", "Unhandled Section");
        }
        return header;
    }
    return QVariant();
}

QModelIndex CardDataModel::index(int row, int column, const QModelIndex &parent) const
{
    Dictionary *dictionary = nullptr;
    if (parent.isValid()) {
        dictionary = static_cast<Dictionary*>(parent.internalPointer());
        if (!dictionary)
            dictionary = m_dictionary;
        dictionary = dictionary->at(parent.row()).value<Dictionary*>();
    }
    return createIndex(row, column, dictionary);
}

QModelIndex CardDataModel::parent(const QModelIndex &index) const
{
    Dictionary *parentDictionary = static_cast<Dictionary*>(index.internalPointer());
    Dictionary *grandParentDictionary = nullptr;
    if (parentDictionary) {
         grandParentDictionary = qobject_cast<Dictionary*>(parentDictionary->parent());
         int parentRow = static_cast<int>(grandParentDictionary->indexOf(QVariant::fromValue(parentDictionary), 0));
         if (grandParentDictionary == m_dictionary)
             grandParentDictionary = nullptr;
         return createIndex(parentRow, 0, grandParentDictionary);
    }
    return QModelIndex();
}

bool CardDataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Dictionary *dictionary = getDictionaryForIndex(index);
    if (role == Qt::CheckStateRole)
        dictionary->set(index.row(), value == Qt::Checked);
    else
        dictionary->set(index.row(), value);
    emit dataChanged(index, index);
    return true;
}

void CardDataModel::updateRow(int row)
{
    QModelIndex modelIndex = index(row, 0);
    emit dataChanged(modelIndex, modelIndex);
}
