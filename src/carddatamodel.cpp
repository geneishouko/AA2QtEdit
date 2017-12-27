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

using namespace ClassEdit;

QVector<QString> CardDataModel::s_keys;
static const int dataColumn = 3;

CardDataModel::CardDataModel(CardFile *cardFile) :
    QAbstractListModel(cardFile),
    m_cardFile(cardFile)
{
    if (s_keys.empty()) {
        s_keys = cardFile->getEditDataKeys();
    }
}

int CardDataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 4;
}

Qt::ItemFlags CardDataModel::flags(const QModelIndex &index) const
{
    if (index.column() != dataColumn)
        return Qt::ItemIsEnabled;
    Qt::ItemFlags f = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
    if (m_cardFile->dataIsBool(index.row()))
        f |= Qt::ItemIsUserCheckable;
    else
        f |= Qt::ItemIsEditable;
    return f;
}

int CardDataModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return s_keys.size();
}

QVariant CardDataModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
            return m_cardFile->getEditDataAddress(s_keys[index.row()]);

            case 1:
            return m_cardFile->getEditDataType(s_keys[index.row()]);

            case 2:
            return s_keys[index.row()];

            case 3:
            if (!m_cardFile->dataIsBool(index.row()))
                return m_cardFile->getEditDataValue(s_keys[index.row()]);
        }
    }
    else if (role == Qt::CheckStateRole) {
        if(index.column() == dataColumn && m_cardFile->dataIsBool(index.row())) {
            return m_cardFile->getEditDataValue(s_keys[index.row()]).toBool() ?
                        Qt::Checked : Qt::Unchecked;
        }
    }
    else if (role == Qt::EditRole) {
        return m_cardFile->getEditDataValue(s_keys[index.row()]);
    }
    else if (role == Qt::BackgroundRole && index.column() != dataColumn) {
        return QBrush(qApp->palette().midlight());
    }
    return QVariant();
}

QVariant CardDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static const char* const headers[] = { "Address", "Type", "Key", "Value" };
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return QString(headers[section]);
    return QVariant();
}

bool CardDataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole)
        m_cardFile->setEditDataValue(s_keys[index.row()], value == Qt::Checked);
    else
        m_cardFile->setEditDataValue(s_keys[index.row()], value);
    emit dataChanged(index, index);
    return true;
}

void CardDataModel::updateAllRows()
{
    emit dataChanged(index(0), index(s_keys.size() - 1));
}

void CardDataModel::updateRow(int row)
{
    QModelIndex modelIndex = index(row);
    emit dataChanged(modelIndex, modelIndex);
}
