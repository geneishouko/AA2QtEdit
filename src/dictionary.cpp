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

#include "dictionary.h"

using namespace ClassEdit;

Dictionary::Dictionary() : QList<QVariant>()
{

}

Dictionary Dictionary::filterByPrefix(const QString &prefix) const
{
    Dictionary dict;
    for (KeyIndex::ConstIterator it = m_keyMap.begin(); it != m_keyMap.end(); it++) {
        if (it.key().startsWith(prefix, Qt::CaseSensitive)) {
            dict.insert(it.key(), at(it.value()));
        }
    }
    return dict;
}

QString Dictionary::keyAt(int index) const
{
    return m_keyMap.key(index);
}

void Dictionary::insert(const QString &key, const QVariant &value)
{
    m_keyMap.insertMulti(key, count());
    append(value);
}

void Dictionary::set(int index, const QVariant &value)
{
    replace(index, value);
}

QVariant Dictionary::value(const QString &key) const
{
    QVariant ret;
    KeyIndex::ConstIterator it = m_keyMap.find(key);
    if (it != m_keyMap.end())
        ret = at(m_keyMap.value(key));
    return ret;
}
