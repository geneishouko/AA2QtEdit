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

#include "datareader.h"
#include "dictionary.h"

using namespace ClassEdit;

Dictionary::Dictionary(QObject *parent) : QObject(parent), QList<QVariant>()
{
    m_dataReader = nullptr;
    m_deleteDataBlocks = false;
}

Dictionary::~Dictionary()
{
    if (m_deleteDataBlocks)
        deleteDataBlocks();
}

void Dictionary::buildDisplayKeyList()
{
    for (int i = 0; i < count(); i++) {
        m_displayKeyList << m_keyMap.key(i);
    }
}

const DataReader *Dictionary::dataReader()
{
    if (m_dataReader)
        return m_dataReader;
    Dictionary *parentDictionary = qobject_cast<Dictionary*>(parent());
    if (parentDictionary)
        m_dataReader = parentDictionary->dataReader();
    return m_dataReader;
}

void Dictionary::deleteDataBlocks()
{
    for (QList<DataBlock*>::ConstIterator it = m_dataBlockList.constBegin(); it != m_dataBlockList.constEnd(); ++it)
        delete *it;
}

QString Dictionary::enumerable(int index)
{
    return dataReader()->getDataEnumerable(m_dataBlockList.at(index)->metaKey())->name(at(index).toInt());
}

QVariantMap Dictionary::filterByPrefix(const QString &prefix) const
{
    QVariantMap dict;
    for (KeyIndex::ConstIterator it = m_keyMap.begin(); it != m_keyMap.end(); it++) {
        if (it.key().startsWith(prefix, Qt::CaseSensitive)) {
            dict.insert(it.key(), at(it.value()));
        }
    }
    return dict;
}

void Dictionary::resetDirtyValues()
{
    auto it = m_dirtyValues.begin();
    while (it != m_dirtyValues.end()) {
        Dictionary *d = at(*it).value<Dictionary*>();
        if (d)
            d->resetDirtyValues();
        it = m_dirtyValues.erase(it);
    }
    Q_ASSERT(!m_dirtyValues.size());
}

void Dictionary::set(int index, const QVariant value)
{
    if (value == at(index))
        return;
    replace(index, value);
    m_dirtyValues << index;
    emit changed(index);
}

void Dictionary::set(const QString &key, QVariant value)
{
    KeyIndex::Iterator it = m_keyMap.find(key);
    if (it != m_keyMap.end())
        set(m_keyMap.value(key), value);
    else
        insert(key, value);
}

void Dictionary::set(const QVariantMap &values)
{
    for (QVariantMap::ConstIterator it = values.begin(); it != values.end(); it++) {
        set(it.key(), it.value());
    }
}

void Dictionary::setDataBlockList(QList<DataBlock *> blockList)
{
    m_dataBlockList = blockList;
    for (int i = 0; i < blockList.size(); i++) {
        if (blockList.at(i)->children().size()) {
            QList<DataBlock *> childList;
            QVector<DataBlock> &origChild = blockList.at(i)->children();
            for (int j = 0; j < origChild.size(); j++) {
                childList.append(&origChild[j]);
            }
            Dictionary *dictionary = at(i).value<Dictionary*>();
            dictionary->setDataBlockList(childList);
        }
    }
}

QString Dictionary::typeName(int index) const
{
    return m_dataBlockList.at(index)->typeName();
}

QVariant Dictionary::value(const QString &key) const
{
    QVariant ret;
    KeyIndex::ConstIterator it = m_keyMap.find(key);
    if (it != m_keyMap.end())
        ret = at(m_keyMap.value(key));
    return ret;
}

void Dictionary::childChanged()
{
    Dictionary *child = qobject_cast<Dictionary*>(sender());
    Q_ASSERT(child);
    int index = indexOf(QVariant::fromValue(child));
    m_dirtyValues << index;
    emit changed(index);
}
