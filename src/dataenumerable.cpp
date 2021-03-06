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

#include "dataenumerable.h"

using namespace ClassEdit;

DataEnumerable::DataEnumerable(const DataType &type) : m_type(type)
{

}

DataType DataEnumerable::type() const
{
    return m_type;
}

void DataEnumerable::registerStringValue(qint32 value, const QString &string)
{
    m_strings.insert(value, string);
}
